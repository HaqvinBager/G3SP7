#include "stdafx.h"
#include "ImguiManager.h"
#include <imgui.h>

#pragma comment(lib, "imgui.lib")

CImguiManager::CImguiManager()
{
	ImGui::DebugCheckVersionAndDataLayout("1.80 WIP", sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(unsigned int));
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
}
