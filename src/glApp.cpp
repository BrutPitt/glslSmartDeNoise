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
#include <chrono>
#include <array>
#include <vector>

#include "glApp.h"
#ifdef GLAPP_TEST
    #include "glWindow_test.h"
#else
    #include "glWindow.h"
#endif

#ifndef __EMSCRIPTEN__
    GLFWmonitor* getCurrentMonitor(GLFWwindow *window);
    void toggleFullscreenOnOff(GLFWwindow* window);
    bool isDoubleClick(int button, int action, double x, double y, double ms);
#endif


// Set the application to null for the linker
mainGLApp* mainGLApp::theMainApp = 0;



static void glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_PRESS)
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
#ifdef GLAPP_USE_IMGUI
        theDlg.visible(theDlg.visible()^1);
#endif
    } //glfwSetWindowShouldClose(window,GLFW_TRUE);
     
    if(key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12 && action == GLFW_PRESS) {
        theWnd->onSpecialKeyDown(key, 0, 0);
#ifndef __EMSCRIPTEN__
        if(key == GLFW_KEY_F11) toggleFullscreenOnOff(window);
#endif

    } else if( key == GLFW_KEY_SPACE && action == GLFW_PRESS) { } //SPACE
    else if(action == GLFW_PRESS) {            
        theWnd->onKeyDown(key==GLFW_KEY_ENTER ? 13 : key, 0, 0);
    }
#ifdef GLAPP_USE_IMGUI
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if(ImGui::GetIO().WantCaptureKeyboard) return;
#endif
}



void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{   
#ifdef GLAPP_USE_IMGUI
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if(ImGui::GetIO().WantCaptureMouse)  return;
#endif

    double x,y;
    glfwGetCursorPos(window, &x, &y);

    if (action == GLFW_PRESS) {
#ifndef __EMSCRIPTEN__
        if(isDoubleClick(button, action, x , y, 250) ) toggleFullscreenOnOff(window);
        else 
#endif            
            theWnd->onMouseButton(button, GLFW_PRESS, x, y); 
        //getApp()->LeftButtonDown();
            
    } else if (action == GLFW_RELEASE) {
#ifndef __EMSCRIPTEN__
        isDoubleClick(button, action, x , y, 250);
#endif            
        theWnd->onMouseButton(button, GLFW_RELEASE, x, y); 
        
        //getApp()->LeftButtonUp();
    }


}


static void glfwCharCallback(GLFWwindow* window, unsigned int c)
{
#ifdef GLAPP_USE_IMGUI
    ImGui_ImplGlfw_CharCallback(window, c);
    if(ImGui::GetIO().WantCaptureKeyboard) return;
#endif
}

void glfwScrollCallback(GLFWwindow* window, double x, double y)
{
#ifdef GLAPP_USE_IMGUI
    ImGui_ImplGlfw_ScrollCallback(window, x, y);
    if(ImGui::GetIO().WantCaptureMouse) return;
#endif
}


static void glfwMousePosCallback(GLFWwindow* window, double x, double y)
{
        //ImGui::GetIO().MousePos.x = (float) x;
        //ImGui::GetIO().MousePos.y = (float) y;

    if((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS) || 
       (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) )
        theWnd->onMotion(x, y); 
}


void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{

    theWnd->onReshape(width,height);
}


#ifndef __EMSCRIPTEN__

bool isDoubleClick(int button, int action, double x, double y, double ms)
{
    static auto before = std::chrono::system_clock::now();
    static int oldAction = -1;
    static int oldButton = -1;
    static double oldx=-1, oldy=-1;    

    auto now = std::chrono::system_clock::now();
    double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();

    bool retval = (diff_ms<ms && (oldx==x && oldy==y) && 
                  (oldButton == button && button == GLFW_MOUSE_BUTTON_LEFT) && 
                  (action==GLFW_PRESS && oldAction==GLFW_RELEASE));
        
    before = now;    
    oldAction=action;
    oldButton = button;
    oldx=x, oldy=y;

    return retval;

}

void toggleFullscreenOnOff(GLFWwindow* window)
{
    static int windowed_xpos, windowed_ypos, windowed_width, windowed_height;
                
    if (glfwGetWindowMonitor(window))
    {
        glfwSetWindowMonitor(window, NULL,
                                windowed_xpos, windowed_ypos,
                                windowed_width, windowed_height, 0);
    }
    else
    {
        GLFWmonitor* monitor = getCurrentMonitor(window);
        if (monitor)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwGetWindowPos(window, &windowed_xpos, &windowed_ypos);
            glfwGetWindowSize(window, &windowed_width, &windowed_height);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    }

}

GLFWmonitor* getCurrentMonitor(GLFWwindow *window)
{
    int nmonitors, i;
    int wx, wy, ww, wh;
    int mx, my, mw, mh;
    int overlap, bestoverlap;
    GLFWmonitor *bestmonitor;
    GLFWmonitor **monitors;
    const GLFWvidmode *mode;

    bestoverlap = 0;
    bestmonitor = NULL;

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++) {
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap =
            std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx)) *
            std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }

    return bestmonitor;
}
#endif

#ifdef GLAPP_USE_IMGUI

// ImGui utils
/////////////////////////////////////////////////
void mainGLApp::imguiInit()
{
    // Setup ImGui binding
    ImGui::CreateContext();

#ifdef GLAPP_IMGUI_VIEWPORT
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcon;
#endif

    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(mainGLFWwnd, false);
    ImGui_ImplOpenGL3_Init(USE_GLSL_VERSION);
    //ImGui::StyleColorsDark();
    setGUIStyle();
}

int mainGLApp::imguiExit()
{
// need to test exit wx ... now 0!
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    return 0;
}
#endif

// glfw utils
/////////////////////////////////////////////////
void mainGLApp::glfwInit()
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!::glfwInit()) exit(EXIT_FAILURE);
#ifdef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
    #ifdef GLAPP_USES_ES3
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        #ifdef GLAPP_REQUIRE_OGL45
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        #else
            #ifdef NDEBUG
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            #else
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // to use DebugCallback
            #endif
        #endif
    #endif

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // from GLFW: If OpenGL ES is requested, this hint is ignored.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // from GLFW: If OpenGL ES is requested, this hint is ignored.
#endif    

#if defined(GLAPP_USES_ES3)
    // From GLFW:
    //      X11: On some Linux systems, creating contexts via both the native and EGL APIs in a single process
    //           will cause the application to segfault. Stick to one API or the other on Linux for now.
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API); //GLFW_NATIVE_CONTEXT_API
#endif

    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 0); // rendering is on FBO, so disable DEPTH buffer of context

#ifdef NDEBUG
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_CONTEXT_NO_ERROR);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif


    setGLFWWnd(glfwCreateWindow(GetWidth(), GetHeight(), getWindowTitle(), NULL, NULL));
    if (!getGLFWWnd())
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //secondary = glfwCreateWindow(512, 512, "My Engine", NULL, getGLFWWnd());

    glfwMakeContextCurrent(getGLFWWnd());

    //glfwSetWindowOpacity(getGLFWWnd(),.5);

#ifdef __EMSCRIPTEN__
/*
    emscripten_set_mousedown_callback("#canvas", nullptr, true, 
        [](int, const EmscriptenMouseEvent* e, void*)->EMSCRIPTEN_RESULT {
            if ((e->button >= 0) && (e->button < 3)) {
                ImGui::GetIO().MouseDown[e->button] = true;
            }
            return true;
        });
    emscripten_set_mouseup_callback("#canvas", nullptr, true, 
        [](int, const EmscriptenMouseEvent* e, void*)->EMSCRIPTEN_RESULT {
            if ((e->button >= 0) && (e->button < 3)) {
                ImGui::GetIO().MouseDown[e->button] = false;
            }
            return true;
        });
*/
#else
    #ifdef GLAPP_USES_ES3
        gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress); //get OpenGL ES extensions
    #else
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);    //get OpenGL extensions
    #endif

#ifndef NDEBUG
/*
    glEnable              ( GL_DEBUG_OUTPUT );
    //glDebugMessageCallback( MessageCallback, nullptr );
    glDebugMessageCallback( openglCallbackFunction, nullptr);
*/
    if(glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
    }
    else
        cout << "glDebugMessageCallback not available: need OpenGL ES 3.2+ or OpenGL 4.3+" << endl;
#endif

#endif



    glfwSetKeyCallback(getGLFWWnd(), glfwKeyCallback);
    glfwSetCharCallback(getGLFWWnd(), glfwCharCallback);
    glfwSetMouseButtonCallback(getGLFWWnd(), glfwMouseButtonCallback);
    glfwSetScrollCallback(getGLFWWnd(), glfwScrollCallback);
    glfwSetCursorPosCallback(getGLFWWnd(), glfwMousePosCallback);
    glfwSetWindowSizeCallback(getGLFWWnd(), glfwWindowSizeCallback);

    glfwSwapInterval(0);

}

int mainGLApp::glfwExit()
{

    glfwDestroyWindow(getGLFWWnd());
    glfwTerminate();

// need to test exit glfw ... now 0!
    return 0;

}

int mainGLApp::getModifier() {
    GLFWwindow* window = getGLFWWnd();
    if((glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
            return GLFW_MOD_CONTROL;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
            return GLFW_MOD_SHIFT;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_ALT) == GLFW_PRESS))
            return GLFW_MOD_ALT;
    else return 0;
}


mainGLApp::mainGLApp() 
{    
    // Allocation in main(...)
    mainGLApp::theMainApp = this;
    glEngineWnd = new glWindow; 
}

mainGLApp::~mainGLApp() 
{
    onExit();

    delete glEngineWnd;
}

void mainGLApp::onInit() 
{

    xPosition = yPosition = -1;
    width = RES_X; height = RES_Y;
    windowTitle = "oglApp";

//Init OpenGL


// Imitialize both FrameWorks
    glfwInit();

// Imitialize both GL engine
    glEngineWnd->onInit();

#ifdef GLAPP_USE_IMGUI
    imguiInit();
#endif

}

int mainGLApp::onExit()  
{
    glEngineWnd->onExit();
// Exit from both FrameWorks
#ifdef GLAPP_USE_IMGUI
    imguiExit();
#endif

    glfwExit();

// need to test returns code... now 0!        
    return 0;
}


void newFrame()
{
#ifdef __EMSCRIPTEN__
    glfwPollEvents();
#endif


    theWnd->onIdle();
    theWnd->onRender();

    // Rendering
//        int display_w, display_h;
    //glViewport(0, 0, width, width);
    //glClearColor(0,0,0,0);
    //glClear(GL_COLOR_BUFFER_BIT);

#ifdef GLAPP_USE_IMGUI
    theDlg.renderImGui();
#endif

    glfwSwapBuffers(theApp->getGLFWWnd());

}


void mainGLApp::mainLoop() 
{
    while (!glfwWindowShouldClose(getGLFWWnd())) {          
        //glfwWaitEvents();
        glfwPollEvents();
        if (!glfwGetWindowAttrib(getGLFWWnd(), GLFW_ICONIFIED)) {
            newFrame();

        }
/*
        static int cnt=0;
        glfwMakeContextCurrent(secondary);

        glClearColor(0.0, 0.0, 0.0, 0.1);
        glClear(GL_COLOR_BUFFER_BIT);

        
        glfwSwapBuffers(secondary);

        glfwMakeContextCurrent(getGLFWWnd());
*/

        //if(!(cnt++%10)) 


    }

}
    



/////////////////////////////////////////////////
// classic entry point
int main(int argc, char **argv)
{
//Initialize class e self pointer
    theApp = new mainGLApp;    

    theApp->onInit();

/////////////////////////////////////////////////
// Enter in GL main loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(newFrame,0,true);
#else
    theApp->mainLoop();
#endif

/////////////////////////////////////////////////
// Exit procedures called from theApp destructor
   
    return 0;

}