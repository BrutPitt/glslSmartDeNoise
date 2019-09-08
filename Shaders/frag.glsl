////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2019 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license
//  
////////////////////////////////////////////////////////////////////////////////
#line 17    //#version dynamically inserted 

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
uniform float uDevStd;
uniform vec2 wSize;

#define INV_SQRT_OF_2PI 0.39894228040143267793994605993439  // 1.0/SQRT_OF_2PI
#define INV_PI 0.31830988618379067153776752674503
///////////////////////////////////////////////////////////////////////////////
//
//  deNoise - smartDeNoise
//
//  sampler2D tex     - sampler image / texture
//  vec2 uv           - actual fragment coord
//  float sigma  > 0  - sigma distribution
//  float devStd >= 0 - Standard Deviations 
//      devStd * sigma  -->  radius of the circular kernel
//  float threshold   - Edge sharpening threshold 
//
///////////////////////////////////////////////////////////////////////////////
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
//
///////////////////////////////////////////////////////////////////////////////
//
//  About Standard Deviations
//
// devStd = 1 cover 68% of data
// devStd = 2 cover 95% of data - over 3 times more points to compute
// devStd = 3 cover 99.7% of data - more over 2 times more points 
//
///////////////////////////////////////////////////////////////////////////////

vec4 smartDeNoise(sampler2D tex, vec2 uv, float sigma, float devStd, float threshold)
{
    float radius = round(devStd*sigma);  // devStd = 2 -> 95% or 3 -> 99.7%
    float radQ = radius * radius;
    
    float invSigma = 1.f/sigma;
    float invSigmaQx2 = .5 * invSigma * invSigma;  // 1.0 / (sigma^2 * 2.0)
    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // 1.0 / (sqrt(PI) * sigma)
    
    float invThreshold = 1.f/threshold;
    float invThresholdSqx2 = .5 * invThreshold * invThreshold;     // 1.0 / (sigma^2 * 2.0)
    float invThresholdSqrt2PI = INV_SQRT_OF_2PI * invThreshold;    // 1.0 / (sqrt(2*PI) * sigma)
    
    vec4 centrPx = texture(tex,uv); 
    
    float Zbuff = 0.0;
    vec4 accumBuff = vec4(0.0);
    vec2 size = vec2(textureSize(tex, 0));
    
    for(float x=-radius; x <= radius; x++)	{
        float pt = sqrt(radQ-x*x);
        for(float y=-pt; y <= pt; y++) {
            vec2 d = vec2(x,y)/size;

            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2;
            
            vec4 walkPx =  texture(tex,uv+d); //getColorPix(tex, uv+d);

            vec4 dC = walkPx-centrPx;
            float deltaFactor = exp( -dot(dC, dC) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;
                                 
            Zbuff     += deltaFactor;
            accumBuff += deltaFactor*walkPx;
        }
    }
    return accumBuff/Zbuff;
}

void main(void)
{
    float slide = uSlider *.5 + .5;
    float szSlide = .001;
    vec2 uv = vec2(gl_FragCoord.xy / wSize);    

    color = uv.x<slide-szSlide  ? texture(imageData, vec2(uv.x,1.0-uv.y)) 
          : (uv.x>slide+szSlide ? smartDeNoise(imageData, vec2(uv.x,1.0-uv.y), uSigma, uDevStd, uThreshold) 
          :  vec4(1.0));

} 