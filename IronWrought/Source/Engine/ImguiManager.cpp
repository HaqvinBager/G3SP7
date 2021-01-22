#include "stdafx.h"
#include "ImguiManager.h"
#include "imgui.h"
#include "ImGuiLevelSelect.h"

CImguiManager::CImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

CImguiManager::~CImguiManager()
{
	ImGui::DestroyContext();
}

void CImguiManager::DebugWindow()
{
	if (ImGui::Begin("Debug info"))
	{
		ImGui::Text("Framerate: %.0f", ImGui::GetIO().Framerate);
	}
	ImGui::End();


	mySelectLevelWindow->RenderWindow();
}
