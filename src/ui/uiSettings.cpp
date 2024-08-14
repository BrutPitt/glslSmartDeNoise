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
#ifdef GLAPP_USE_IMGUI

#include <imgui/imgui.h>

#include "../glApp.h"


void darkColorsOnBlack(const ImVec4 &color)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const float kC = .25f;
    const float lum = color.x*.299f + color.y*.587f + color.z*.114f;
    const float lumK = 1 + lum * .25;

    ImVec4 wBG(color.x*kC, color.y*kC, color.z*kC, 1.0f);

    
    ImVec4 clA(wBG.x*4.3f*lumK, wBG.y*4.3f*lumK, wBG.z*4.3f*lumK, 1.00f); //0.00f, 0.55f, 0.87f
    ImVec4 clB(wBG.x*1.5f*lumK, wBG.y*1.5f*lumK, wBG.z*1.5f*lumK, 1.00f); //0.20f, 0.22f, 0.27f
    ImVec4 clC(wBG.x*3.0f*lumK, wBG.y*3.0f*lumK, wBG.z*3.0f*lumK, 1.00f); //0.20f, 0.22f, 0.27f
    ImVec4 clD(wBG.x*3.7f*lumK, wBG.y*3.7f*lumK, wBG.z*3.7f*lumK, 1.00f);

    
    ImVec4 HSV;

    const float diff = 1.f/(lumK*lumK*lumK);
    clD.x *= diff;
    clD.y *= diff;
    clD.z *= diff;

    ImGui::ColorConvertRGBtoHSV(wBG.x, wBG.y, wBG.z, HSV.x, HSV.y, HSV.z);
    if(HSV.y>.25) HSV.y=.25; 
    HSV.z=.85;
    ImVec4 txt(0.90f, 0.90f, 0.90f, 1.00f); //0.86f, 0.93f, 0.89f    

    ImVec4 chk((txt.x+color.x)*.5f, (txt.y+color.y)*.5f, (txt.z+color.z)*.5f, 1.0f) ;

    const ImVec4 ch1(chk.x*.60, chk.y*.60, chk.z*.60, 1.00f);
    const ImVec4 ch2(chk.x*.80, chk.y*.80, chk.z*.80, 1.00f);


    style.Colors[ImGuiCol_Text]                 = ImVec4(txt.x, txt.y, txt.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(txt.x, txt.y, txt.z, 1.00f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(clA.x, clA.y, clA.z, 0.65f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(clD.x, clD.y, clD.z, 0.35f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(clD.x, clD.y, clD.z, 0.66f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(clD.x, clD.y, clD.z, 0.85f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(clB.x, clB.y, clB.z, 0.50f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.00f, 0.00f, 0.00f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(clB.x, clB.y, clB.z, 0.85f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(clB.x, clB.y, clB.z, 0.35f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(clA.x, clA.y, clA.z, 0.44f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(clA.x, clA.y, clA.z, 0.74f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(chk.x, chk.y, chk.z, 0.80f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(clA.x, clA.y, clA.z, 0.36f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(clA.x, clA.y, clA.z, 0.76f);
    style.Colors[ImGuiCol_Button]               = ImVec4(clC.x, clC.y, clC.z, 0.35f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(clA.x, clA.y, clA.z, 0.43f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(clA.x, clA.y, clA.z, 0.62f);
    style.Colors[ImGuiCol_Header]               = ImVec4(clA.x, clA.y, clA.z, 0.33f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(clA.x, clA.y, clA.z, 0.42f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(clA.x, clA.y, clA.z, 0.54f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(clB.x, clB.y, clB.z, 0.50f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(clB.x, clB.y, clB.z, 0.75f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(clC.x, clC.y, clC.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(clA.x, clA.y, clA.z, 0.54f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(clA.x, clA.y, clA.z, 0.74f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(clA.x, clA.y, clA.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(chk.x, chk.y, chk.z, 0.40f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(chk.x, chk.y, chk.z, 0.50f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(clA.x, clA.y, clA.z, 0.22f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(clB.x, clB.y, clB.z, 0.40f);
}




void setGUIStyle(void)
{
    ImGuiStyle& style = ImGui::GetStyle();

    //SetupImGuiStyle2();
    darkColorsOnBlack(ImVec4(.75, 0.0, 0.0, 1.0));
    style.ItemSpacing = ImVec2(2,3); 
    style.ItemInnerSpacing = ImVec2(2,3); 

    style.FrameRounding = 3;
    style.WindowRounding = 5;
    style.ScrollbarSize = 15;
    style.GrabRounding = 3;
    style.WindowPadding = ImVec2(3,3);
    style.ChildRounding = 3;
    style.ScrollbarRounding = 5;
    style.FramePadding = ImVec2(3,3);

    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    //style.FrameBorderSize = .0f;
    
    //style.WindowTitleAlign = ImVec2(.03f,.5f);
    


    ImFontConfig cfg;
    //memset(&cfg, 0, sizeof(ImFontConfig));
    cfg.OversampleH = 3.;
    cfg.OversampleV = 1.;
    cfg.PixelSnapH = true;
    cfg.GlyphExtraSpacing.x = 0.0f;
    cfg.GlyphExtraSpacing.y = 0.0f;
    cfg.RasterizerMultiply = 1.25f;

    //ImVec4 color(style.Colors[ImGuiCol_FrameBg]);

    //ImVec4 v(style.Colors[ImGuiCol_FrameBg]);
//    theDlg.setTableAlterbateColor1(ImVec4(v.x*1.1,v.y*1.1,v.z*1.1,v.w));
    //theDlg.setTableAlterbateColor2(ImVec4(v.x*.9,v.y*.9,v.z*.9,v.w));


}

#endif