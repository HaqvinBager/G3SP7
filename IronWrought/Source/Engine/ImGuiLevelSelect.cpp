#include "stdafx.h"
#include "ImGuiLevelSelect.h"
#include "imgui.h"
#include "JsonReader.h"
#include "SceneManager.h"
#include "Engine.h"
#include "FolderUtility.h"

CImGuiLevelSelect::CImGuiLevelSelect()
{
}

CImGuiLevelSelect::~CImGuiLevelSelect()
{
}

void CImGuiLevelSelect::RenderWindow()
{
    std::vector<std::string> files = CFolderUtility::GetFileNamesInFolder(ASSETPATH("Assets/Generated/"), ".json");
	ImGui::Begin("LevelSelect");
    if (ImGui::TreeNode("Selection State: Single Selection"))
    {
        static int selected = -1;
        for (int n = 0; n < files.size(); n++)
        {
            char buf[512];
            sprintf_s(buf, "%s", files[n].c_str());


            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                std::cout << "Load Level: " << buf << std::endl;
                CScene* myUnityScene = CSceneManager::CreateScene(buf);
                CEngine::GetInstance()->AddScene(CStateStack::EState::InGame, myUnityScene);
                CEngine::GetInstance()->SetActiveScene(CStateStack::EState::InGame);
            }

            if (ImGui::Selectable(buf, selected == n))
            {
                selected = n;

            
            }
        }
        ImGui::TreePop();
    }


	ImGui::End();
}
