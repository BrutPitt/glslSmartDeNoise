# glslSmartDeNoise

Fast glsl spatial **deNoise** filter, with circular gaussian kernel and smart/flexible -> full configurable:

- Standard Deviation sigma radius
- K factor sigma coefficient
- Edge sharpening threshold

[![](https://raw.githubusercontent.com/BrutPitt/glslSmartDeNoise/master/sShot.jpg)](https://raw.githubusercontent.com/BrutPitt/glslSmartDeNoise/master/sShot.jpg)

### Live WebGL2 demo

You can run/test **WebGL 2** examples of **imGuIZMO** from following links:
- **[glslSmartDeNoise](https://brutpitt.github.io/glslSmartDeNoise/WebGL/wglApp.html)**


It works only on browsers with **WebGl 2** and **webAssembly** support (FireFox/Opera/Chrome and Chromium based)

Test if your browser supports WebGL 2, here: [WebGL2 Report](http://webglreport.com/?v=2)

**glslSmartDeNoise** is used in **[glChAoS.P](https://github.com/BrutPitt/glChAoS.P)** poroject to produce a GLOW effect like a "stardust" or "particle-dust" (is the *"bilinear threshold"* filter in the GLOW section)

## glslSmartDeNoise filter
Below there is the filter source code with parameters and optimizations description.

To examine its use, you can watch the `Shader\frag.glsl` file, while all other files are only part of the **C++** example

``` glsl
#define INV_SQRT_OF_2PI 0.39894228040143267793994605993439  // 1.0/SQRT_OF_2PI
#define INV_PI 0.31830988618379067153776752674503

//  smartDeNoise - parameters
///////////////////////////////////////////////////////////////////////////////
//
//  sampler2D tex     - sampler image / texture
//  vec2 uv           - actual fragment coord
//  float sigma  >  0 - sigma Standard Deviation
//  float kSigma >= 0 - sigma coefficient 
//      kSigma * sigma  -->  radius of the circular kernel
//  float threshold   - Edge sharpening threshold 

vec4 smartDeNoise(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)
{
    float radius = round(kSigma*sigma);  // devStd = 2 -> 95% or 3 -> 99.7%
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

//  About Standard Deviations
///////////////////////////////////////////////////////////////////////////////
//
//  kSigma = 1*sigma cover 68% of data
//  kSigma = 2*sigma cover 95% of data - over 3 times more points to compute
//  kSigma = 3*sigma cover 99.7% of data - more over 2 times more points 


//  Optimizations (description)
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

```
**can find it also in* `Shader/frag.glsl` *file*

### Building Example

The C++ example shown in the screenshot is provided.
To build it you can use CMake (3.10 or higher) or the Visual Studio solution project (for VS 2017/2019) in Windows.
You need to have installed [**GLFW**](https://www.glfw.org/) in your compiler search path (LIB/INCLUDE).
Other tools: [**ImGui**](https://github.com/ocornut/imgui) and [**lodePNG**](https://github.com/lvandeve/lodepng) are attached, and already included in the `CMakeList.txt`

*Currently `CMakeList.txt` was tested only to build ONLY [**EMSCRIPTEN**](https://kripken.github.io/emscripten-site/index.html) example, other O.S. will be tested soon.*

The CMake file is able to build also an [**EMSCRIPTEN**](https://kripken.github.io/emscripten-site/index.html) version, obviously you need to have installed EMSCRIPTEN SDK on your computer (1.38.10 or higher): look at or use the helper batch/script files, in main example folder, to pass appropriate defines/patameters to CMake command line.

To build the EMSCRIPTEN version, in Windows, with CMake, need to have **mingw32-make.exe** in your computer and search PATH (only the make utility is enough): it is a condition of EMSDK tool to build with CMake in Windows.


**For windows users that use vs2017 project solution:**
The current VisualStudio project solution refers to my environment variable RAMDISK (`R:`), and subsequent VS intrinsic variables to generate binary output:
`$(RAMDISK)\$(MSBuildProjectDirectoryNoRoot)\$(DefaultPlatformToolset)\$(Platform)\$(Configuration)\` 
Even without a RAMDISK variable, executable and binary files are outputted in base to the values of these VS variables, starting from root of current drive.

