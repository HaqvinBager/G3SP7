#include "stdafx.h"
#include "PlayerControlWindow.h"

#include "Engine.h"
#include "Scene.h"
#include <PlayerControllerComponent.h>

namespace IronWroughtImGui
{
	CPlayerControlWindow::CPlayerControlWindow(const char* aName)
		: CWindow(aName)
		, myPlayerCrouchSpeed(0.03f)
		, myPlayerWalkSpeed(0.06f)
		, myPlayerJumpHeight(0.00f)
		, myPlayerFallSpeed(0.00f)
		, myPlayerShortcut(nullptr)
	{
		
	}
	CPlayerControlWindow::~CPlayerControlWindow()
	{}
	void CPlayerControlWindow::OnEnable()
	{
		myPlayerShortcut = IRONWROUGHT_ACTIVE_SCENE.Player()->GetComponent<CPlayerControllerComponent>();
		myPlayerCrouchSpeed = myPlayerShortcut->CrouchSpeed();
		myPlayerWalkSpeed = myPlayerShortcut->WalkSpeed();
		myPlayerJumpHeight = myPlayerShortcut->JumpHeight();
		myPlayerFallSpeed = myPlayerShortcut->FallSpeed();
	}
	void CPlayerControlWindow::OnInspectorGUI()
	{
		ImGui::Begin(Name(), Open());
		{
			ImGui::DragFloat("Walk Speed",	 &myPlayerWalkSpeed,   0.001f, 0.0f, 5.0f);
			ImGui::DragFloat("Crouch Speed", &myPlayerCrouchSpeed, 0.001f, 0.0f, 5.0f);
			ImGui::DragFloat("Jump Height",  &myPlayerJumpHeight,  0.001f, 0.0f, 5.0f);
			ImGui::DragFloat("Fall Speed",   &myPlayerFallSpeed,   0.001f, 0.0f, 5.0f);
			
			myPlayerShortcut->WalkSpeed(myPlayerWalkSpeed);
			myPlayerShortcut->CrouchSpeed(myPlayerCrouchSpeed);
			myPlayerShortcut->JumpHeight(myPlayerJumpHeight);
			myPlayerShortcut->FallSpeed(myPlayerFallSpeed);


			ImGui::SetWindowSize(ImVec2(0.f, 0.f));
		}
		ImGui::End();
	}
	void CPlayerControlWindow::OnDisable()
	{}
}