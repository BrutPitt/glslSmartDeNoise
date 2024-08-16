//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#line 13    //#version dynamically inserted

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  glslSmartDeNoise
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

out vec4 color;

#ifdef GL_ES
    uniform sampler2D imageData;
#else
    #if (__VERSION__>=450)
        layout (binding=1) uniform sampler2D imageData;
    #else 
        uniform sampler2D imageData;
    #endif
#endif

uniform float uSigma;
uniform float uThreshold;
uniform float uSlider;
uniform float uKSigma;
uniform float lowHi;
uniform vec2 wSize;

#define INV_SQRT_OF_2PI 0.39894228040143267793994605993439  // 1.0/SQRT_OF_2PI
#define INV_PI          0.31830988618379067153776752674503

//  Parameters:
//      sampler2D tex     - sampler image / texture
//      vec2 uv           - actual fragment coord
//      float sigma  >  0 - sigma Standard Deviation
//      float kSigma >= 0 - sigma coefficient
//          kSigma * sigma  -->  radius of the circular kernel
//      float threshold   - edge sharpening threshold
vec4 smartDeNoise(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)
{
    float radius = round(kSigma*sigma);
    float radQ = radius * radius;

    float invSigmaQx2 = .5 / (sigma * sigma);      // 1.0 / (sigma^2 * 2.0)
    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // // 1/(2 * PI * sigma^2)

    float invThresholdSqx2 = .5 / (threshold * threshold);     // 1.0 / (sigma^2 * 2.0)
    float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;   // 1.0 / (sqrt(2*PI) * sigma)

    vec4 centrPx = texture(tex,uv);

    float zBuff = 0.0;
    vec4 aBuff = vec4(0.0);
    vec2 size = vec2(textureSize(tex, 0));

    vec2 d;
    for (d.x=-radius; d.x <= radius; d.x++) {
        float pt = sqrt(radQ-d.x*d.x);       // pt = yRadius: have circular trend
        for (d.y=-pt; d.y <= pt; d.y++) {
            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI;

            vec4 walkPx =  texture(tex,uv+d/size);
            
            vec4 dC = walkPx-centrPx;
            float deltaFactor = exp( -dot(dC.rgb, dC.rgb) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;

            zBuff += deltaFactor;
            aBuff += deltaFactor*walkPx;
        }
    }
    return aBuff/zBuff;
}

//  About Standard Deviations (watch Gauss curve)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  kSigma = 1*sigma cover 68% of data
//  kSigma = 2*sigma cover 95% of data - but there are over 3 times 
//                   more points to compute
//  kSigma = 3*sigma cover 99.7% of data - but needs more than double 
//                   the calculations of 2*sigma


//  Optimizations (description)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  fX = exp( -(x*x) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 
//  fY = exp( -(y*y) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 
//  where...
//      invSigmaSqx2     = 1.0 / (sigma^2 * 2.0)
//      invSigmaxSqrt2PI = 1.0 / (sqrt(2 * PI) * sigma)
//
//  now, fX*fY can be written in unique expression...
//
//      e^(a*X) * e^(a*Y) * c*c
//
//      where:
//        a = invSigmaSqx2, X = (x*x), Y = (y*y), c = invSigmaxSqrt2PI
//
//           -[(x*x) * 1/(2 * sigma^2)]             -[(y*y) * 1/(2 * sigma^2)] 
//          e                                      e
//  fX = -------------------------------    fY = -------------------------------
//                ________                               ________
//              \/ 2 * PI  * sigma                     \/ 2 * PI  * sigma
//
//      now with... 
//        a = 1/(2 * sigma^2), 
//        X = (x*x) 
//        Y = (y*y) ________
//        c = 1 / \/ 2 * PI  * sigma
//
//      we have...
//              -[aX]              -[aY]
//        fX = e      * c;   fY = e      * c;
//
//      and...
//                 -[aX + aY]    [2]     -[a(X + Y)]    [2]
//        fX*fY = e           * c     = e            * c   
//
//      well...
//
//                    -[(x*x + y*y) * 1/(2 * sigma^2)]
//                   e                                
//        fX*fY = --------------------------------------
//                                        [2]           
//                          2 * PI * sigma           
//      
//      now with assigned constants...
//
//          invSigmaQx2   = 1/(2 * sigma^2)
//          invSigmaQx2PI = 1/(2 * PI * sigma^2) = invSigmaQx2 * INV_PI 
//
//      and the kernel vector 
//
//          k = vec2(x,y)
//
//      we can write:
//
//          fXY = exp( -dot(k,k) * invSigmaQx2) * invSigmaQx2PI

uniform float invGamma;
uniform bool useTest;
uniform int whichTest;

vec4 smartDeNoiseLowHi(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)
{
    float radius = round(kSigma*sigma);
    float radQ = radius * radius;

    float invSigmaQx2 = .5 / (sigma * sigma);      // 1.0 / (sigma^2 * 2.0)
    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // // 1/(2 * PI * sigma^2)

    float invThresholdSqx2 = .5 / (threshold * threshold);     // 1.0 / (sigma^2 * 2.0)
    float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;   // 1.0 / (sqrt(2*PI) * sigma)

    vec4 centrPx = texture(tex,uv);
    vec4 centrPxNeg = vec4(pow(centrPx.rgb, vec3(invGamma)), centrPx.a);

    float zBuff = 0.0, zBuffNeg = 0.0;
    vec4 aBuff = vec4(0.0);
    vec4 aBuffNeg = vec4(0.0);
    vec2 size = vec2(textureSize(tex, 0));

    vec2 d;
    for (d.x=-radius; d.x <= radius; d.x++) {
        float pt = sqrt(radQ-d.x*d.x);       // pt = yRadius: have circular trend
        for (d.y=-pt; d.y <= pt; d.y++) {
            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI;

            vec4 walkPx =  texture(tex,uv+d/size);
            vec4 walkPxNeg =  walkPx;

            vec4 dC = walkPx-centrPx;
            vec4 dCNeg = walkPxNeg-centrPxNeg;

            float K = invThresholdSqrt2PI * blurFactor;

            float deltaFactor = exp( -dot(dC.rgb, dC.rgb) * invThresholdSqx2) * K;
            float deltaFactorNeg = exp( -dot(dCNeg.rgb, dCNeg.rgb) * invThresholdSqx2) * K;

            zBuffNeg += deltaFactorNeg;
            aBuffNeg += deltaFactorNeg*walkPxNeg;

            zBuff += deltaFactor;
            aBuff += deltaFactor*walkPx;
        }
    }
    return mix(aBuff/zBuff, aBuffNeg/zBuffNeg, lowHi);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  End glslSmartDeNoise
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


float smartDeNoise_GrayScaleImage(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)
{
    float radius = round(kSigma*sigma);
    float radQ = radius * radius;

    float invSigmaQx2 = .5 / (sigma * sigma);      // 1.0 / (sigma^2 * 2.0)
    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // // 1/(2 * PI * sigma^2)

    float invThresholdSqx2 = .5 / (threshold * threshold);     // 1.0 / (sigma^2 * 2.0)
    float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;   // 1.0 / (sqrt(2*PI) * sigma)

    float centrPx = texture(tex,uv).r;

    float zBuff = 0.0;
    float aBuff = 0.0;
    vec2 size = vec2(textureSize(tex, 0));

    vec2 d;
    for (d.x=-radius; d.x <= radius; d.x++) {
        float pt = sqrt(radQ-d.x*d.x);       // pt = yRadius: have circular trend
        for (d.y=-pt; d.y <= pt; d.y++) {
            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI;

            float walkPx =  texture(tex,uv+d/size).r;

            float dC = walkPx-centrPx;
            float deltaFactor = exp( -(dC*dC) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;

            zBuff += deltaFactor;
            aBuff += deltaFactor*walkPx;
        }
    }
    return aBuff/zBuff;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Testing other color spaces
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifdef GL_ES
#define saturate(v) clamp(v, vec3(0.0), vec3(1.0))
#else
#ifndef saturate
#define saturate(v) clamp(v, 0.0, 1.0)
#endif
#endif

//  Used in Example: sRGB space
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vec4 smartDeNoise_sRGB(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)
{
    float radius = round(kSigma*sigma);
    float radQ = radius * radius;

    float invSigmaQx2 = .5 / (sigma * sigma);      // 1.0 / (sigma^2 * 2.0)
    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // // 1/(2 * PI * sigma^2)

    float invThresholdSqx2 = .5 / (threshold * threshold);     // 1.0 / (sigma^2 * 2.0)
    float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;   // 1.0 / (sqrt(2*PI) * sigma)

    vec3 centrPx = pow(texture(tex,uv).rgb, vec3(invGamma));

    float zBuff = 0.0;
    vec3 aBuff = vec3(0.0);
    vec2 size = vec2(textureSize(tex, 0));

    vec2 d;
    for (d.x=-radius; d.x <= radius; d.x++) {
        float pt = sqrt(radQ-d.x*d.x);       // pt = yRadius: have circular trend
        for (d.y=-pt; d.y <= pt; d.y++) {
            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI;

            vec3 walkPx = texture(tex,uv+d/size).rgb;

            vec3 dC = pow(walkPx, vec3(invGamma))-centrPx;
            float deltaFactor = exp( -dot(dC, dC) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;

            zBuff += deltaFactor;
            aBuff += deltaFactor*walkPx;
        }
    }
    return vec4(aBuff/zBuff, 1.0);
}


//  Used in Example: HSL space
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vec4 smartDeNoise_HSL(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)
{
    float radius = round(kSigma*sigma);
    float radQ = radius * radius;

    float invSigmaQx2 = .5 / (sigma * sigma);      // 1.0 / (sigma^2 * 2.0)
    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // // 1/(2 * PI * sigma^2)

    float invThresholdSqx2 = .5 / (threshold * threshold);     // 1.0 / (sigma^2 * 2.0)
    float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;   // 1.0 / (sqrt(2*PI) * sigma)


    vec3 c = texture(tex,uv).rgb;
    vec3 centrHSL = rgb2hsl(c);

    float zBuff = 0.0;
    vec4 aBuff = vec4(0.0);
    vec2 size = vec2(textureSize(tex, 0));

    vec2 d;
    for (d.x=-radius; d.x <= radius; d.x++) {
        float pt = sqrt(radQ-d.x*d.x);       // pt = yRadius: have circular trend
        for (d.y=-pt; d.y <= pt; d.y++) {
            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI;

            vec4 walkPx =  texture(tex,uv+d/size);
            vec3 walkPxHSL = rgb2hsl(walkPx.rgb);

            vec3 dC = walkPxHSL - centrHSL;
            float deltaFactor = exp( -dot(dC.xz, dC.xz) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;

            zBuff += deltaFactor;
            aBuff += deltaFactor*walkPx;
        }
    }
    return aBuff/zBuff;
}

//  Used in Example: Luminance space
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vec4 smartDeNoise_lum(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)
{
    float radius = round(kSigma*sigma);
    float radQ = radius * radius;

    float invSigmaQx2 = .5 / (sigma * sigma);      // 1.0 / (sigma^2 * 2.0)
    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // // 1/(2 * PI * sigma^2)

    float invThresholdSqx2 = .5 / (threshold * threshold);     // 1.0 / (sigma^2 * 2.0)
    float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;   // 1.0 / (sqrt(2*PI) * sigma)


    vec3 c = texture(tex,uv).rgb;
    float centrLum = whichTest == 1 ? luminance(c) : linearLum(c);

    float zBuff = 0.0;
    vec4 aBuff = vec4(0.0);
    vec2 size = vec2(textureSize(tex, 0));

    vec2 d;
    for (d.x=-radius; d.x <= radius; d.x++) {
        float pt = sqrt(radQ-d.x*d.x);       // pt = yRadius: have circular trend
        for (d.y=-pt; d.y <= pt; d.y++) {
            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI;

            vec4 walkPx =  texture(tex,uv+d/size);

            float dC = (whichTest == 1 ? luminance(walkPx.rgb) : linearLum(walkPx.rgb)) - centrLum;
            float deltaFactor = exp( -(dC * dC) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;

            zBuff += deltaFactor;
            aBuff += deltaFactor*walkPx;
        }
    }
    return aBuff/zBuff;
}


vec4 smartDeNoise_test(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)
{
    switch(whichTest) {
        default:
        case 0:
            //return smartDeNoiseLowHi(imageData, uv, uSigma, uKSigma, uThreshold);
        return vec4(smartDeNoise_GrayScaleImage(imageData, uv, uSigma, uKSigma, uThreshold), vec3(0.0));

        case 1: //sRGB
            return smartDeNoise_sRGB(imageData, uv, uSigma, uKSigma, uThreshold);
        case 2: //luminance
        case 3: //linear Luminance
            return smartDeNoise_lum(imageData, uv, uSigma, uKSigma, uThreshold);
        case 4:
            return smartDeNoise_HSL(imageData, uv, uSigma, uKSigma, uThreshold);
    }
}

void main(void)
{
    float slide = uSlider *.5 + .5;
    float szSlide = .001;
    vec2 uv = vec2(gl_FragCoord.xy / wSize);

    vec4 c = useTest ?  smartDeNoise_test(imageData, vec2(uv.x,1.0-uv.y), uSigma, uKSigma, uThreshold) :
                        smartDeNoise(imageData, vec2(uv.x,1.0-uv.y), uSigma, uKSigma, uThreshold);

    color = uv.x<slide-szSlide  ? texture(imageData, vec2(uv.x,1.0-uv.y)) :
                                 (uv.x>slide+szSlide ? c : vec4(1.0));


} 