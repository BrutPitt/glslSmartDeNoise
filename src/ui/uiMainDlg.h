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

#ifdef GLAPP_USE_IMGUI

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


class mainImGuiDlgClass
{
public:
    mainImGuiDlgClass() { isVisible = true; }

    //~mainImGuiDlgClass() {}


    void renderImGui();
/*
    void setTableAlterbateColor1(ImVec4 &c) { tableAlternateColor1 = c; }
    ImVec4 &getTableAlterbateColor1() { return tableAlternateColor1; }
    void setTableAlterbateColor2(ImVec4 &c) { tableAlternateColor2 = c; }
    ImVec4 &getTableAlterbateColor2() { return tableAlternateColor2; }
*/
    void visible(bool b) { isVisible = b; }
    bool visible() { return isVisible; }

private:
    bool isVisible;

private:
//    ImVec4 tableAlternateColor1, tableAlternateColor2;

};

#endif