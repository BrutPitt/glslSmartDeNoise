//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Copyright (c) 2018-2019 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://BrutPitt.com
//
//  me@michelemorrone.eu - brutpitt@gmail.com
//  twitter: @BrutPitt - github: BrutPitt
//  
//  https://github.com/BrutPitt/glslSmartDeNoise/
//
//  This software is distributed under the terms of the BSD 2-Clause license
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma once

#define GLAPP_USE_IMGUI

//#define GLAPP_REQUIRE_OGL45

//#include "glUtilities.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
    #include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

#include <tools/glslProgramObject.h>

#ifdef GLAPP_USE_IMGUI
    #include "ui/uiMainDlg.h"
#endif

#define GLAPP_SHADERS "Shaders/"

#ifdef __EMSCRIPTEN__
    #define USE_GLSL_VERSION   "#version 300 es\n\n"
    #define USE_GLSL_PRECISION "precision highp float;\n"
#else
    #ifdef GLAPP_REQUIRE_OGL45 
        #define USE_GLSL_VERSION   "#version 450\n"
    #else
        #define USE_GLSL_VERSION   "#version 410\n"
    #endif
    #define USE_GLSL_PRECISION 
#endif


#ifndef theApp
    #define theApp mainGLApp::theMainApp
#endif
#ifndef theWnd
    #define theWnd theApp->getEngineWnd()
#endif
#ifndef theDlg
    #define theDlg theApp->getMainDlg()
#endif


enum ScreeShotReq {
    ScrnSht_NO_REQUEST,
    ScrnSht_SILENT_MODE,
    ScrnSht_FILE_NAME
};

#define RES_X 1280
#define RES_Y 800


/**
 * An glApp class encapsulating the windowing system.
 *
 * \author Don Olmstead
 * \version 1.0
 */

class glWindow;

void setGUIStyle();

/////////////////////////////////////////////////
// theApp -> Main App -> container
/////////////////////////////////////////////////


class mainGLApp
{
public:
    // self pointer .. static -> the only one 
    static mainGLApp* theMainApp;

    mainGLApp();
    ~mainGLApp();

    void onInit();
    int onExit();

    void mainLoop();
////////////////////////////////
//GLFW Utils
    GLFWwindow* getGLFWWnd()  const { return(mainGLFWwnd);  }

    glWindow *getEngineWnd() { return glEngineWnd; }
    void setGLFWWnd(GLFWwindow* wnd) { mainGLFWwnd = wnd; }

	int getXPosition() const { return(xPosition); }
    int getYPosition() const { return(yPosition); }
	int GetWidth()     const { return(width);     }
	int GetHeight()    const { return(height);    }
    void SetWidth(int v)  { width  = v; }
    void SetHeight(int v) { height = v; }
	const char* getWindowTitle() const { return(windowTitle.c_str()); }

protected:

		// The Position of the window
		int xPosition, yPosition;
		int width = RES_X, height = RES_Y;
		/** The title of the window */

        bool exitFullScreen;

		// The title of the window
		std::string windowTitle;
    
    
private:
#ifdef GLAPP_USE_IMGUI
// imGui utils
/////////////////////////////////////////////////
    void imguiInit();
    int imguiExit();
    mainImGuiDlgClass mainImGuiDlg;
public:
    mainImGuiDlgClass &getMainDlg() { return mainImGuiDlg; }
#endif
private:

// glfw utils
/////////////////////////////////////////////////
    void glfwInit();
    int glfwExit();
    int getModifier();

    GLFWwindow* mainGLFWwnd;
    glWindow *glEngineWnd;

friend class glWindow;

};



