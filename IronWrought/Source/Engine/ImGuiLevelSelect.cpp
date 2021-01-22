#include "stdafx.h"
#include "ImGuiLevelSelect.h"
#include "imgui.h"

CImGuiLevelSelect::CImGuiLevelSelect()
{
}

CImGuiLevelSelect::~CImGuiLevelSelect()
{
}

void CImGuiLevelSelect::RenderWindow()
{
    //MenuBar Opens this Window?

    /*
        LoadLevel
            <Read all Files from Generated Folder>

    */


	ImGui::Begin("LevelSelect");

    if (ImGui::TreeNode("Selection State: Single Selection"))
    {
        static int selected = -1;
        for (int n = 0; n < 5; n++)
        {
            char buf[32];
            sprintf_s(buf, "Object %d", n);
            if (ImGui::Selectable(buf, selected == n))
                selected = n;
        }
        ImGui::TreePop();
    }


	ImGui::End();
}
