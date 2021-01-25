#include "stdafx.h"
#include "ImGuiLevelSelect.h"
#include "imgui.h"
#include "JsonReader.h"
#include "SceneManager.h"
#include "Engine.h"

CImGuiLevelSelect::CImGuiLevelSelect() : mySelectedScene(0)
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

	std::vector<std::string> files = CJsonReader::GetFilePathsInFolder(ASSETPATH + "Assets/Generated");
	if (ImGui::BeginCombo("Scene", files[mySelectedScene].c_str()))
	{
		for (int n = 0; n < files.size(); n++)
		{
			char buf[512];
			sprintf_s(buf, "%s", files[n].c_str());
			if (ImGui::Selectable(buf, mySelectedScene == n))
			{
				mySelectedScene = n;
				LoadLevel(files[n]);
			}
		}
		ImGui::EndCombo();
	}
}

void CImGuiLevelSelect::LoadLevel(std::string aLevelPath)
{

	std::cout << "Load: " << aLevelPath << std::endl;
}
