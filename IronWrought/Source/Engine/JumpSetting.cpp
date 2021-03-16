#include "stdafx.h"
#include "JumpSetting.h"
#include "Engine.h"
#include "Scene.h"
#include <PlayerControllerComponent.h>

ImGuiWindow::CJumpSetting::CJumpSetting(const char* aName)
	: CWindow(aName)
	, myJumpHeight(0.025f)
{
}

ImGuiWindow::CJumpSetting::~CJumpSetting()
{
}

void ImGuiWindow::CJumpSetting::OnEnable()
{
}

void ImGuiWindow::CJumpSetting::OnInspectorGUI()
{


	ImGui::Begin(Name(), Open());

	if (ImGui::SliderFloat("Jump Height", &myJumpHeight, 0.01f, 0.05f)) {
		

		CPlayerControllerComponent* player = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CPlayerControllerComponent>();

		if (player != nullptr)
		{
			player->SetPlayerJumpHeight(myJumpHeight);

		}
		else
		{
			assert(false && "No player found in this scene");
		}
	}


	ImGui::End();
}

void ImGuiWindow::CJumpSetting::OnDisable()
{
}
