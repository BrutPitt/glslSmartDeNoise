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
#pragma once

#include "glApp.h"

/*
///////////////////////////////////////////
//Data Init for 32/64 bit systems
//////////////////////////////////////////

template<int> void IntDataHelper();

template<> void IntDataHelper<4>() 
{
  // do 32-bits operations
}

template<> void IntDataHelper<8>() 
{
  // do 64-bits operations
}

// helper function just to hide clumsy syntax
inline void IntData() { IntDataHelper<sizeof(size_t)>(); }
*/

#define NUM_TEXTURES 4

#include <cstdint>
#if INTPTR_MAX == INT32_MAX 
    //APP compiling 32bit    
#elif INTPTR_MAX == INT64_MAX
    //APP compiling 32bit
#else
    #error "Environment not 32 or 64-bit."
#endif

enum wTest { LowHi, sRGB, luminance, linearLuminance, HSL };

class vaoClass {
public:
    vaoClass() {}

    void init() {
        float vtx[] = {-1.0f,-1.0f,
                        1.0f,-1.0f,
                        1.0f, 1.0f,
                       -1.0f, 1.0f };


        size = sizeof(vtx);
#ifdef GLAPP_REQUIRE_OGL45
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vaoBuffer);
        glNamedBufferStorage(vaoBuffer, size, vtx, 0); 

        glVertexArrayAttribBinding(vao,vPosition, 0);
        glVertexArrayAttribFormat(vao, vPosition, 2, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(vao, vPosition);        

        glVertexArrayVertexBuffer(vao, 0, vaoBuffer, 0, 8);

#else
        glGenVertexArrays(1, &vao); 
        glGenBuffers(1, &vaoBuffer);
        glBindBuffer(GL_ARRAY_BUFFER,vaoBuffer);
        glBufferData(GL_ARRAY_BUFFER,size, vtx, GL_STATIC_READ);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vaoBuffer);
        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0L);
        glEnableVertexAttribArray(vPosition);
        CHECK_GL_ERROR();
#endif
    }

    ~vaoClass() {
        glDeleteBuffers(1, &vaoBuffer);
        glDeleteVertexArrays(1, &vao);
    }

    void draw() {
        glBindVertexArray(vao);        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        CHECK_GL_ERROR();
    }
private:
    GLuint vao, vaoBuffer;
    enum Attrib_IDs { vPosition = 0 };
    int size;
};

class textureBaseClass 
{
public:
    
    textureBaseClass() { }

    GLuint getTexID()   { return texID[whichImg];   }
    GLuint getTexSize() { return texSize; }
    void buildTex(int idx, void* buffer, const int x, const int y);

    GLuint texID[NUM_TEXTURES]  ;
    GLuint texSize =  0;
    GLint whichImg = 1;

protected:
    bool generated = false;
    void genTex(int idx);
    void assignAttribs(int idx, GLint filterMin=GL_LINEAR, GLint filterMag=GL_LINEAR, GLint wrap=GL_CLAMP_TO_EDGE);

};


class deNoiseClass : public mainProgramObj //, public uniformBlocksClass
{
struct uDataStruct {
} uData;
public:
    void initShader()
    {
        useVertex(); useFragment();

        getVertex  ()->Load(USE_GLSL_VERSION USE_GLSL_PRECISION, 1, GLAPP_SHADERS "vert.glsl");
        getFragment()->Load(USE_GLSL_VERSION USE_GLSL_PRECISION, 2, GLAPP_SHADERS "colorSpaces.glsl", GLAPP_SHADERS "frag.glsl");
        // The vertex and fragment are added to the program object
        addVertex();
        addFragment();
    
        link();

        locSigma = glGetUniformLocation(program, "uSigma");
        locThreshold = glGetUniformLocation(program, "uThreshold");
        locWSize = glGetUniformLocation(program, "wSize");
        locSlider = glGetUniformLocation(program, "uSlider");
        locKSigma = glGetUniformLocation(program, "uKSigma");
        locInvGamma = glGetUniformLocation(program, "invGamma");
        locUseTest = glGetUniformLocation(program, "useTest");
        locWhichTest = glGetUniformLocation(program, "whichTest");
        locLowHi = glGetUniformLocation(program, "lowHi");

#if !defined(GLAPP_REQUIRE_OGL45)
        bindShaderProg();
        locImgData = glGetUniformLocation(program,"imageData");

#endif

        //uniformBlocksClass::create(GLuint(sizeof(uMandelData)), (void *) &uData);

        //buildColors();
        vao.init();

    }


    textureBaseClass& getTexture() { return texture; }
    void render();


    void rescale(int w, int h)
    {
        glViewport(0,0,w,h);
        theApp->SetWidth(w); theApp->SetHeight(h);
    }



    uDataStruct &getUData() { return uData; }

    //GLfloat sigma = 11.0f, threshold = .180f,  //running
    // GLfloat sigma = 7.0f, threshold = .180f, slider = 0.f; //running
    //GLfloat kSigma = 2.f;

    GLfloat slider = 0.f;

    GLfloat aSigma[NUM_TEXTURES] = {9.f, 7.f, 7.f, 7.f};
    GLfloat aThreshold[NUM_TEXTURES] = {.180f, .195f, .180f, .1f};
    GLfloat aKSigma[NUM_TEXTURES] = {3.f, 3.f, 2.f, 3.f};
    GLfloat gamma = 2.2;
    GLfloat lowHi = .5;
    bool useTest = false;
    GLint whichTest = 0;

private:
    textureBaseClass texture;

    GLuint locSigma,locThreshold, locWSize, locSlider, locKSigma, locInvGamma, locUseTest, locWhichTest, locImgData, locLowHi;
    vaoClass vao;

};




using namespace std;

class glApp;


void setViewOrtho();


typedef float tbT;

class glWindow /*: public glApp*/
{
    mainGLApp *GetFrame()  { return theApp; }
    mainGLApp *GetCanvas() { return theApp; }   

public:        

    glWindow();
    virtual ~glWindow();


    // Called when Window is created
    virtual void onInit();
    // Called when Window is closed.
    virtual void onExit();

    // The onIdle and onDisplay methods should also be overloaded.
    // Within the onIdle method put the logic of the application.
    // The onDisplay method is for any drawing code.
    virtual void onIdle();
    virtual void onRender();
    virtual void onReshape(GLint w, GLint h);

    virtual void onMouseButton(int button, int upOrDown, int x, int y);
    virtual void onMouseWheel(int wheel, int direction, int x, int y);
    virtual void onMotion(int x, int y);
    virtual void onPassiveMotion(int x, int y);

    // The onKeyDown method handles keyboard input that are standard ASCII keys
    virtual void onKeyDown(unsigned char key, int x, int y);
    virtual void onKeyUp(unsigned char key, int x, int y);
    virtual void onSpecialKeyUp(int key, int x, int y);
    virtual void onSpecialKeyDown(int key, int x, int y);


    int GetWidth()  { return GetCanvas()->GetWidth();  }
    int GetHeight() { return GetCanvas()->GetHeight(); }

    deNoiseClass& getDeNoise() { return *deNoise; }

private:

    deNoiseClass *deNoise;

};

