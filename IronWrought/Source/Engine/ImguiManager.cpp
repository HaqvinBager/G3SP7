#include "stdafx.h"
#include "ImguiManager.h"
#include "imgui.h"

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
	static char buffer[1024];
	if (ImGui::Begin("Debug info"))
	{
		ImGui::Text("Framerate: %.0f", ImGui::GetIO().Framerate);
	}
	ImGui::End();
}
