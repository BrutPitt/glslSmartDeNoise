# glslSmartDeNoise

Fast glsl spatial **deNoise** filter, with circular gaussian kernel and smart/flexible -> full configurable:

- Sigma radius
- Standard Deviations
- Edge sharpening threshold

[![](https://raw.githubusercontent.com/BrutPitt/glslSmartDeNoise/master/sShot.jpg)](https://raw.githubusercontent.com/BrutPitt/glslSmartDeNoise/master/sShot.jpg)

Filter is all here
``` glsl
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
    vec2 size = textureSize(tex, 0);
    
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

```

### Live WebGL2 demo

You can run/test **WebGL 2** examples of **imGuIZMO** from following links:
- **[glslSmartDeNoise](https://brutpitt.github.io/glslSmartDeNoise/WebGL/wglApp.html)**


There is way to test the filter changing parameters.

It works only on browsers with **WebGl 2** and **webAssembly** support (FireFox/Opera/Chrome and Chromium based)

Test if your browser supports WebGL 2, here: [WebGL2 Report](http://webglreport.com/?v=2)

**glslSmartDeNoise** is used in **[glChAoS.P](https://github.com/BrutPitt/glChAoS.P)** poroject to produce a GLOW effect like a "stardust" or "particle-dust" (is the *"bilinear threshold"* filter in the GLOW section)


### Building Example

The C++ example shown in the screenshot is provided.
To build it you can use CMake (3.10 or higher) or the Visual Studio solution project (for VS 2017/2019) in Windows.
You need to have installed [**GLFW**](https://www.glfw.org/) in your compiler search path (LIB/INCLUDE).
Other tools: [**ImGui**](https://github.com/ocornut/imgui) and [**lodePNG**](https://github.com/lvandeve/lodepng) are attached, and already included in the project/CMakeList.txt

The CMake file is able to build also an [**EMSCRIPTEN**](https://kripken.github.io/emscripten-site/index.html) version, obviously you need to have installed EMSCRIPTEN SDK on your computer (1.38.10 or higher): look at or use the helper batch/script files, in main example folder, to pass appropriate defines/patameters to CMake command line.

To build the EMSCRIPTEN version, in Windows, with CMake, need to have **mingw32-make.exe** in your computer and search PATH (only the make utility is enough): it is a condition of EMSDK tool to build with CMake in Windows.


**For windows users that use vs2017 project solution:**
The current VisualStudio project solution refers to my environment variable RAMDISK (`R:`), and subsequent VS intrinsic variables to generate binary output:
`$(RAMDISK)\$(MSBuildProjectDirectoryNoRoot)\$(DefaultPlatformToolset)\$(Platform)\$(Configuration)\` 
Even without a RAMDISK variable, executable and binary files are outputted in base to the values of these VS variables, starting from root of current drive.

