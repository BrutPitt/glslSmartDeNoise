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

#ifdef GLAPP_USE_IMGUI

#include <sstream>
//#include <glm/gtc/type_ptr.hpp>



#include "../glApp.h"

#ifdef GLAPP_TEST
    #include "../glWindow_test.h"
#else
    #include "../glWindow.h"
#endif


#include "uiMainDlg.h"


    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>
using namespace ImGui;

void mainImGuiDlgClass::renderImGui()
{

    static bool metricW = false, demosW = false;
    if(!visible()) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuiStyle& style = ImGui::GetStyle();
    float sizeY = theWnd->getDeNoise().useTest ? 12 : 9;
           


   ImGui::SetNextWindowSize(ImVec2(250, ImGui::GetFrameHeightWithSpacing()*sizeY+5), ImGuiCond_Always);
   ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_FirstUseEver);


    bool isVisible = true;
        
    if(ImGui::Begin("glslSmartDeNoise", &isVisible)) {
        ImGui::BeginGroup(); {

            const float w = ImGui::GetContentRegionAvail().x;
            const float third = w/3.f;

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Avg:  %.3f ms/f  -  %.1f fps", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::NewLine();
/*
            ImGui::Text("DevStd ");
            ImGui::SameLine();
            float x = ImGui::GetCursorPosX();
            ImGui::PushItemWidth(half-x);
            if (ImGui::Combo("##DevStd", &theWnd->getDeNoise().devStd,  "1*sigma\0"\
                                                                        "2*sigma\0"\
                                                                        "3*sigma\0"));
*/
            int idx = theWnd->getDeNoise().getTexture().whichImg;
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Radius=");
            ImGui::SameLine();
            float x = ImGui::GetCursorPosX();
            const float half = (w-x)/2.f - 6;
            ImGui::PushItemWidth(half);
            //ImGui::DragFloat("##DevStd",&theWnd->getDeNoise().devStd,.05,1.0,3.0,"DevStd: %.3f");
            ImGui::SliderFloat("##DevStd",&theWnd->getDeNoise().aKSigma[idx],0.0,3.0,"DevStd %.2f");
            ImGui::PopItemWidth();

            ImGui::SameLine();
            ImGui::Text("x");
            ImGui::SameLine();
            ImGui::PushItemWidth(half);
            ImGui::DragFloat("##Sigma",&theWnd->getDeNoise().aSigma[idx],.025,0.001f,24.0,"sigma %.3f");
            ImGui::PopItemWidth();

            ImGui::PushItemWidth(w);
            ImGui::DragFloat("##Threshold",&theWnd->getDeNoise().aThreshold[idx],.001,.001f,2.0,"edge threshold %.3f");


            ImGui::NewLine();
            ImGui::SliderFloat("##Slide",&theWnd->getDeNoise().slider,-1.0,1.0,"slide %.3f");
            ImGui::PopItemWidth();
            ImGui::Combo(" Images", &theWnd->getDeNoise().getTexture().whichImg,
                                         "Tree - daylight\0"\
                                         "Tree - sunset\0"\
                                         "Runner - on the beach\0");

            NewLine();
            ImGui::Checkbox(" Test other spaces...", &theWnd->getDeNoise().useTest);

            if(theWnd->getDeNoise().useTest) {
                NewLine();
                ImGui::PushItemWidth(w);
                ImGui::Combo("##SpaceType", &theWnd->getDeNoise().whichTest,
                                             "sRGB (gamma correction)\0"\
                                             "Luminance\0"\
                                             "linear Luminance\0"\
                                             "HSL \0");
                                             
                if(theWnd->getDeNoise().whichTest == wTest::sRGB) {
                    ImGui::DragFloat("##gamma",&theWnd->getDeNoise().gamma, .01, 1.0, 15.0, "gamma %.3f");
//                    ImGui::SameLine();
                }
                ImGui::PopItemWidth();
            }


/*
            static vec2 aaa[] = { {0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f}, 
                                  {0.1f,0.3f}, 
                                  {0.1f,0.3f}, 
                                  {0.1f,0.3f}, 
                                  {0.5f,0.6f}, 
                                  {0.5f,0.6f}, 
                                  {0.5f,0.6f}, 
                                  {1.0f,1.0f}, 
                                  {1.0f,1.0f}, 
                                  {1.0f,1.0f}, 
                                  {2.0f,2.0f}, 
                                  {2.0f,2.0f}, 
                                  {2.0f,2.0f}, 
                                  {2.1f,2.3f}, 
                                  {2.1f,2.3f}, 
                                  {2.1f,2.3f}, 
                                  {2.5f,2.6f}, 
                                  {2.5f,2.6f}, 
                                  {2.5f,2.6f}, 
                                  {3.0f,3.0f} };
            int newVals;
            CurveEditor("prova", glm::value_ptr(aaa[0]), 6, ImVec2(w,ImGui::GetFrameHeightWithSpacing()*15), 0, &newVals); 
*/
            
            } ImGui::EndGroup(); 

        } ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

#endif