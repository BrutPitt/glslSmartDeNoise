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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <lodePNG/lodepng.h>

#include "glWindow.h"


void textureBaseClass::genTex()
{

    if(generated) {
#if !defined(GLCHAOSP_LIGHTVER)
        glDeleteTextures(1,&texID);
        CHECK_GL_ERROR();
#else
        return;
#endif
    }

#ifdef GLAPP_REQUIRE_OGL45
    glCreateTextures(GL_TEXTURE_2D, 1, &texID);
#else
    glGenTextures(1, &texID);   // Generate OpenGL texture IDs
#endif

    generated = true;
}

void textureBaseClass::assignAttribs(GLint filterMin, GLint filterMag, GLint wrap) 
{
#ifdef GLAPP_REQUIRE_OGL45
    glTextureParameteri(texID, GL_TEXTURE_MAG_FILTER, filterMag);
    glTextureParameteri(texID, GL_TEXTURE_MIN_FILTER, filterMin);
    glTextureParameteri(texID, GL_TEXTURE_WRAP_S, wrap );
    glTextureParameteri(texID, GL_TEXTURE_WRAP_T, wrap );
#else
    glBindTexture(GL_TEXTURE_2D, texID);			// Bind Our Texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
#endif
}

void textureBaseClass::buildTex(void *buffer, const ivec2 &size)
{
    genTex();
    const GLint texInternal = GL_RGBA;

    glActiveTexture(GL_TEXTURE0 + texID);
    glBindTexture(GL_TEXTURE_2D, texID);			// Bind Our Texture
    glTexImage2D(GL_TEXTURE_2D, 0, texInternal, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    assignAttribs(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    CHECK_GL_ERROR();

}


void deNoiseClass::render()
{

    glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f)));

    bindPipeline();

    glUseProgram(program);

    glUniform1f(locSigma, sigma); 
    glUniform1f(locThreshold, threshold);
    glUniform1f(locSlider, slider); 
    glUniform1f(locKSigma, kSigma); 
    glUniform2f(locWSize,theApp->GetWidth(),theApp->GetHeight());

#ifdef __EMSCRIPTEN__
    // pass uniform variables
#else
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(1, theWnd->getDeNoise().getTexture().getTexID());
#else
    glActiveTexture(GL_TEXTURE0+theWnd->getDeNoise().getTexture().getTexID());
    glBindTexture(GL_TEXTURE_2D, theWnd->getDeNoise().getTexture().getTexID());
#endif
#endif
    //updateBufferData();
    // Draw
    vao.draw();


}



using namespace glm;

void setView()
{

   
}


void setViewOrtho()
{


}


glWindow::glWindow()
{
    

}


glWindow::~glWindow()
{

}



//
/////////////////////////////////////////////////
void glWindow::onInit()
{
    std::vector<unsigned char> image;
    unsigned x,y;

    lodepng::decode(image, x, y, "imageD.png");
    ivec2 size(x,y);


    //glViewport(0, 0, theApp->GetWidth(), theApp->GetWidth());
    theApp->SetWidth(x); theApp->SetHeight(y);
    printf("%d, %d\n",x,y);
    glViewport(0, 0, x, y);
    glfwSetWindowSize(theApp->getGLFWWnd(),x,y);
    getDeNoise().getTexture().buildTex(image.data(), size);
    deNoise.initShader();
}



//
/////////////////////////////////////////////////
void glWindow::onExit()
{
    
}

//
/////////////////////////////////////////////////
void glWindow::onRender()
{
    deNoise.render();
}



//
/////////////////////////////////////////////////
void glWindow::onIdle()
{
}


//
/////////////////////////////////////////////////
void glWindow::onReshape(GLint w, GLint h)
{
    deNoise.rescale(w,h);


}

//
/////////////////////////////////////////////////
void glWindow::onKeyUp(unsigned char key, int x, int y)
{

}


//
/////////////////////////////////////////////////
void glWindow::onSpecialKeyDown(int key, int x, int y)
{


}


//
/////////////////////////////////////////////////
void glWindow::onKeyDown(unsigned char key, int x, int y)
{



}



//
/////////////////////////////////////////////////
void glWindow::onSpecialKeyUp(int key, int x, int y)
{



}


//
/////////////////////////////////////////////////
void glWindow::onMouseButton(int button, int upOrDown, int x, int y)
{


}

//
/////////////////////////////////////////////////
void glWindow::onMouseWheel(int wheel, int direction, int x, int y)
{

}

//
/////////////////////////////////////////////////
void glWindow::onMotion(int x, int y)
{
    
}

//
/////////////////////////////////////////////////
void glWindow::onPassiveMotion(int x, int y)
{

}
