cmake_minimum_required(VERSION 3.15)
# SET(BUILD_wglAPP FALSE CACHE FALSE TRUE)
###########################################################
#
# To build example in Linux/MacOS/Windows (with MinGW) uses follow command:
#
# cmake -DBuildTarget:String=<BuildVer> -G <MakeTool> -B<FolderToBuild>
#   where:
#       <BuildVer> must be one of follow strings:
#           OpenGL_45
#           OpenGL_41
#           OpenGL_ES
#       <MakeTool> is your preferred generator like "Unix Makefiles" or "Ninja"
#       <FolderToBuild> is the folder where will be generated Makefile, move in it and run your generator
#       - Default build is "Release" but it can be changed via CMAKE_BUILD_TYPE definition:
#           command line: -DCMAKE_BUILD_TYPE:STRING=<Debug|Release|MinSizeRel|RelWithDebInfo>
#           cmake-gui: from combo associated to CMAKE_BUILD_TYPE var
#
# Example:
#   to build example compliant to OpenGL 4.5, with "make" utility, in "./build" folder, type:
#
#       > cmake -DBuildTarget:String=OpenGL_45 -G Unix\ Makefiles -B./build
#       > cd build
#       > make
#
#
#
#
# To build example with EMSCRIPTEN uses follow command:
# cmake cmake -DCMAKE_TOOLCHAIN_FILE:STRING=<EMSDK_PATH>/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DBuildTarget:String=<BuildVer> -G "Unix Makefiles"|"ninja" -B<FolderToBuild>
#   where:
#       <EMSDK_PATH> is where was installed EMSCRIPTEN: you need to have it in EMSDK environment variable
#       <BuildVer> must be one of follow strings:
#           WebGL
#           WebGL_LowRes
#       <MakeTool> is your preferred generator like "Unix Makefiles" or "ninja"
#           Windows users need to use MinGW-make utility (by EMSCRIPTEN specification): ninja or othe can not work.
#       <FolderToBuild> is the folder where will be generated Makefile, move in it and run your generator
#       - Default build is "MinSizeRel" but it can be changed via CMAKE_BUILD_TYPE definition:
#           command line: -DCMAKE_BUILD_TYPE:STRING=<Debug|Release|MinSizeRel|RelWithDebInfo>
#           cmake-gui: from combo associated to CMAKE_BUILD_TYPE var
#


#set(GLAPP_DISABLE_DLA "-DGLAPP_DISABLE_DLA")

set(OGL_45      "OpenGL_45")
set(OGL_41      "OpenGL_41")
set(OGL_ES      "OpenGL_ES")
set(WGL_FULL    "WebGL")

set(BuildTarget ${OGL_45} CACHE STRING "Select the target to build:\n    glChAoS.P compliant with OpenGL 4.5/4.1/ES 3.x\n    wglChAoS.P compliant with WebGL 2.0\n    \"LowRes\" stands for GPU with LowResources (mem/cores\)")
set(BuildTargetValues "${OGL_45};${OGL_41};${OGL_ES};${WGL_FULL};" CACHE INTERNAL "Select one")
set_property(CACHE BuildTarget PROPERTY STRINGS ${BuildTargetValues} )

set(OGL45 "-DGLAPP_REQUIRE_OGL45") #-DGLAPP_USES_SEPARABLE

set(OGL41 "")
set(OGLES "-DGLAPP_USES_ES3 -DIMGUI_IMPL_OPENGL_ES3")

set(BASE_DEFINES "-DGLAPP_USE_IMGUI -DVGM_USES_TEMPLATE")


if(${BuildTarget} STREQUAL ${OGL_45})
    set(OGL_VER_USED ${OGL45})
    message(STATUS "selected ${BuildTarget} ...")
    elseif(${BuildTarget} STREQUAL ${OGL_41})
        set(OGL_VER_USED ${OGL41})
        message(STATUS "selected ${BuildTarget} ...")
    elseif(${BuildTarget} STREQUAL ${OGL_ES})
            set(OGL_VER_USED ${OGLES})
            message(STATUS "selected ${BuildTarget} ...")
    elseif(${BuildTarget} STREQUAL ${WGL_FULL})
                set(BUILD_WebGL TRUE BOOL)
                set(defaultBuildType "MinSizeRel")
                message(STATUS "selected ${BuildTarget} ...")
endif()

#change default build type from cmake-gui
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "'CMAKE_BUILD_TYPE' not specified, now is set to: '${defaultBuildType}' ...")
  set(CMAKE_BUILD_TYPE "${defaultBuildType}" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;MinSizeRel;RelWithDebInfo")
endif()


set(SRC "${CMAKE_SOURCE_DIR}/src")

    set(SOURCE_FILES
        ${SRC}/libs/imgui/imconfig.h
        ${SRC}/libs/imgui/imgui.cpp
        ${SRC}/libs/imgui/imgui_widgets.cpp
        ${SRC}/libs/imgui/imgui.h
        ${SRC}/libs/imgui/imgui_draw.cpp
        ${SRC}/libs/imgui/imgui_impl_glfw.cpp
        ${SRC}/libs/imgui/imgui_impl_glfw.h
        ${SRC}/libs/imgui/imgui_impl_opengl3.cpp
        ${SRC}/libs/imgui/imgui_tables.cpp
        ${SRC}/libs/imgui/imgui_impl_opengl3_loader.h
        ${SRC}/libs/imgui/imgui_impl_opengl3.h
        ${SRC}/libs/imgui/imgui_impl_opengl3.h
        ${SRC}/libs/imgui/imgui_internal.h
        ${SRC}/libs/imgui/imstb_rectpack.h
        ${SRC}/libs/imgui/imstb_textedit.h
        ${SRC}/libs/imgui/imstb_truetype.h
        ${SRC}/libs/lodePNG/lodepng.cpp
        ${SRC}/libs/lodePNG/lodepng.h
        ${SRC}/tools/glslProgramObject.cpp
        ${SRC}/tools/glslProgramObject.h
        ${SRC}/tools/glslShaderObject.cpp
        ${SRC}/tools/glslShaderObject.h
        ${SRC}/ui/uiMainDlg.cpp
        ${SRC}/ui/uiMainDlg.h
        ${SRC}/ui/uiSettings.cpp
        ${SRC}/glApp.cpp
        ${SRC}/glApp.h
        ${SRC}/glWindow.cpp
        ${SRC}/glWindow.h)

###########################################################
#   EMSCRIPTEN setting
###########################################################
if(BUILD_WebGL)
    project(wglApp)
    set(OGL_EMITTER_TYPE ${OGL41})
    
    set(CMAKE_CXX_STANDARD 14)
    find_package(OpenGL)

    include_directories(BEFORE SYSTEM ${EMSCRIPTEN_ROOT_PATH}/system/include)
#    include_directories(BEFORE SYSTEM $ENV{EMSCRIPTEN}/system/include)
    include_directories(${SRC})
    include_directories(${SRC}/tools)
    include_directories(AFTER ${SRC}/libs)
#    include_directories(AFTER $ENV{INCLUDE})

# -o ${PROJECT_NAME}.html ->  CMAKE_EXECUTABLE_SUFFIX  .html .bc, .js (default), etc
set(CMAKE_EXECUTABLE_SUFFIX ".html")

# relative emscripten output files (EMSOUT_PATH for my RAMDISK, if defined)
    if(EXISTS $ENV{EMSOUT_PATH})
        set(EMSOUT "$ENV{EMSOUT_PATH}/emsOut")
    else()
        set(EMSOUT "${CMAKE_SOURCE_DIR}/emsOut")
    endif()

    file(MAKE_DIRECTORY ${EMSOUT})
    file(MAKE_DIRECTORY ${EMSOUT}/${CMAKE_BUILD_TYPE})
    set(EMSOUT ${EMSOUT}/${CMAKE_BUILD_TYPE})

# emcc compiler options
    set(M_EMCC_FLAGS "-DGLAPP_NO_OGL_DSA -DGLFW_INCLUDE_ES3")
    set(M_GLOBAL_FLAGS "${BASE_DEFINES} -DIMGUI_IMPL_OPENGL_ES3 -fpermissive")

# BUILD TYPE: [Debug|RelWithDebInfo|Release|MinSizeRel]    
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(M_EMCC_BINARYEN "-s WASM=0 -s DEMANGLE_SUPPORT=1 ")
        set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/shell_minimal.html\"")
    else()
        #  -s BINARYEN_METHOD='native-wasm,asmjs' 'interpret-asm2wasm' -s BINARYEN_ASYNC_COMPILATION=0
        if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
            set(M_EMCC_BINARYEN "-s WASM=1")
            set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/shell_minimal.html\"")
        else()
            if(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
                # one file no memory option: -s SINGLE_FILE=1 --memory-init-file 0
                set(M_EMCC_BINARYEN "--no-heap-copy --closure 1 -s WASM=1 ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/mVeryMinimal.html\"")
            else() # Release 
                # -s SINGLE_FILE=1 --memory-init-file 0 -s \"BINARYEN_METHOD='interpret-asm2wasm'\"
                set(M_EMCC_BINARYEN "-s WASM=1 ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/mVeryMinimal.html\"")
            endif()    
        endif()    
    endif()

    set(M_EMCC_EMBED_ALL_ATTRACTORS "${CMAKE_SOURCE_DIR}/../ChaoticAttractors@ChaoticAttractors/")
    set(M_EMCC_EMBED_FEW_ATTRACTORS "${CMAKE_SOURCE_DIR}/../selectedAttractors@ChaoticAttractors/")

# grouped EMCC_OPTIONS
#     --embed-file \"${CMAKE_SOURCE_DIR}/../ChaoticAttractors@ChaoticAttractors/\"\

    set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/Shaders@Shaders/\"\
     --preload-file \"${CMAKE_SOURCE_DIR}/image0.png@./\"")
     
#     --embed-file \"${CMAKE_SOURCE_DIR}/../Fonts/Cousine-Regular.ttf@Fonts/\"\
#     --embed-file \"${CMAKE_SOURCE_DIR}/../Fonts/fontawesome-webfont.ttf@Fonts/\"\

    # set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/Shaders@/Shaders\"")
    # -s FULL_ES3=1 -s OFFSCREENCANVAS_SUPPORT=1
    set(M_EMCC_OGL_OPTIONS "-s USE_WEBGL2=1 -s USE_GLFW=3 -s LEGACY_GL_EMULATION=0 -s OFFSCREENCANVAS_SUPPORT=1 ")

# merge all EMCC_OPTIONS -s BINARYEN_TRAP_MODE=clamp 
    set(M_EMCC_OPTIONS "-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ALLOW_MEMORY_GROWTH=1 -s BINARYEN_TRAP_MODE=clamp -s PRECISE_F32=1 ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EMSOUT}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS} ${OGL_EMITTER_TYPE} ${M_EMCC_FLAGS} ${M_EMCC_OPTIONS} ${GLAPP_DISABLE_DLA} -Wno-parentheses -Wno-format-security -Wno-logical-op-parentheses -Wno-unused-value -Wdelete-non-virtual-dtor")

    add_executable(${PROJECT_NAME} 
                    ${SOURCE_FILES})


###########################################################
#   PC setting
###########################################################
else()
    set(APP_NAME "SmartDeNoise")
    if(APPLE)
        project(gl${APP_NAME}_OSX)
    else()
        if(WIN32)
            project(gl${APP_NAME})
        else()
            project(gl${APP_NAME}_Linux)
        endif()
    endif()

    set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME}
        ${SRC}/libs/glad/glad.cpp
        ${SRC}/libs/glad/glad.h
        ${SOURCE_FILES})

# GLFW package
#
    find_package(glfw3)
    if(glfw3_FOUND)
        set(LIB_GLFW "glfw")
    else()
        message (FATAL_ERROR "GLFW not found... REQUIRED!!!!")
    endif()

# OpenGL
#
    find_package(OpenGL)
if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")
    #include_directories(${OPENGL_INCLUDE_DIRS}) # no more necessary: modern OGL uses loaders ==> glad
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif()

if(EXISTS $ENV{RAMDISK}) #my RAMDISK env var
    message("RamDisk...")
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $ENV{RAMDISK}/gl${APP_NAME})
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/..)
endif()

# set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/builds")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

if(APPLE)
    set(COMMON_DEFINES "${BASE_DEFINES} -DGLFW_INCLUDE_NONE -DIMGUI_IMPL_OPENGL_LOADER_GLAD -Wno-logical-op-parentheses -Wno-unused-command-line-argument -Wno-parentheses -framework Cocoa -framework IOKit -framework CoreVideo")
else()
    set(COMMON_DEFINES "${BASE_DEFINES} -DGLFW_INCLUDE_NONE -DIMGUI_IMPL_OPENGL_LOADER_GLAD")
endif(APPLE)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security ${OGL_EMITTER_TYPE} ${COMMON_DEFINES} ${GLAPP_DISABLE_DLA} -std=c++14 -fpermissive")

include_directories(${SRC})
include_directories(${SRC}/tools)
include_directories(${SRC}/libs)
# include_directories($ENV{INCLUDE})

#uncomment for GLFW installed dir

if(OPENGL_FOUND)
    if(APPLE)
        set(TARGET_LIBS "-ldl")
    else()
        add_definitions(${OGL_VER_USED})
        if(WIN32)
        set(TARGET_LIBS "-ldl")
        else()    
        set(TARGET_LIBS "-ldl -lX11")
        endif(WIN32)
    endif(APPLE)

    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} ${TARGET_LIBS} ${LIB_GLFW})
endif(OPENGL_FOUND)

endif() 
