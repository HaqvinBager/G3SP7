#include "stdafx.h"
#include "ImGuiStateMachine.h"
#include "StateMachineComponent.h"

#include "Engine.h"
#include "Scene.h"

#include <imgui.h>
ImGuiStateMachine::ImGuiStateMachine()
	: myIsEnabled(false)
	, myAddStateData()
{
}

ImGuiStateMachine::~ImGuiStateMachine()
{
}

void ImGuiStateMachine::OnInspectorGUI()
{
	CScene& scene = CEngine::GetInstance()->GetActiveScene();
	auto stateMachine = scene.FindFirstObjectWithComponent<CStateMachineComponent>();

	ImGui::Begin("State Machine Editor");
	
	ShowAddStateButton(stateMachine);


	CStateMachineComponent::SerializedObject data = stateMachine->GetData();
	for (const auto& state : data.myStates) {
		ImGui::Begin(state->myName.c_str());

		ImGui::End();
	}	


	ImGui::End();
}

void ImGuiStateMachine::ShowAddStateButton(CStateMachineComponent* aStateMachine)
{
	if (myAddStateData.myAddStateFlag) {

		if (ImGui::InputText("State Name:", myAddStateData.bufName, 64)) {
			myAddStateData.myAddStateNameFlag = true;
		}

		if (myAddStateData.myAddStateNameFlag) {
			if (ImGui::Button("Add")) {

				SState* newState = new SState();
				newState->myName = std::string(myAddStateData.bufName);
				aStateMachine->AddState(newState);

				myAddStateData.myAddStateFlag = false;
				myAddStateData.myAddStateNameFlag = false;
			}
		}
	}
	else {
		if (ImGui::Button("Add State")) {
			ZeroMemory(myAddStateData.bufName, 64);

			myAddStateData.myAddStateFlag = true;
		}
	}
}
