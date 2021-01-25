#include "stdafx.h"
#include "ImguiManager.h"
#include "imgui.h"

#include "RenderManager.h"
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

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
		ImGui::Text(GetSystemMemory().c_str());
		ImGui::Text(GetDrawCalls().c_str());
	}
	ImGui::End();
}

const std::string CImguiManager::GetSystemMemory()
{
	// From TGA2D
	PROCESS_MEMORY_COUNTERS memCounter;
	BOOL result = GetProcessMemoryInfo(GetCurrentProcess(),
		&memCounter,
		sizeof(memCounter));

	if (!result)
	{
		return "";
	}

	SIZE_T memUsed = (memCounter.WorkingSetSize) / 1024;
	SIZE_T memUsedMb = (memCounter.WorkingSetSize) / 1024 / 1024;

	std::string mem = "System Memory: ";
	mem.append(std::to_string(memUsed));
	mem.append("Kb (");
	mem.append(std::to_string(memUsedMb));
	mem.append("Mb)");

	return mem;
}

const std::string CImguiManager::GetDrawCalls()
{
	std::string drawCalls = "Draw Calls: ";
	drawCalls.append(std::to_string(CRenderManager::myNumberOfDrawCallsThisFrame));
	return drawCalls;
}
