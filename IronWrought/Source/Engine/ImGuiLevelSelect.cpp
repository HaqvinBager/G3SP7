#include "stdafx.h"
#include "ImGuiLevelSelect.h"
#include "imgui.h"
#include "JsonReader.h"
#include "SceneManager.h"
#include "Engine.h"

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

    //Access all .json files inside ASSETPATH + Assets/Generated/ Folder

    std::vector<std::string> files = CJsonReader::GetFilePathsInFolder(ASSETPATH + "Assets/Generated");
	ImGui::Begin("LevelSelect");
    if (ImGui::TreeNodeEx("Scenes"))
    {
        static int selected = -1;
        for (int n = 0; n < files.size(); n++)
        {
            char buf[512];
            sprintf_s(buf, "%s", files[n].c_str());
            if (ImGui::Selectable(buf, selected == n, ImGuiSelectableFlags_AllowDoubleClick))
            {
                selected = n;
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    std::cout << "Load level" << buf << std::endl;
                }
            }
        }
        ImGui::TreePop();
    }
	ImGui::End();
}
