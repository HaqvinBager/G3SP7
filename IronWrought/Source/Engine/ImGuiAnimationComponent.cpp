#include "stdafx.h"
#include "ImGuiAnimationComponent.h"

#include "Engine.h"
#include "Scene.h"
#include "AnimationComponent.h"

ImGuiAnimationComponent::ImGuiAnimationComponent(const char* aWindowName)
	: CImGuiWindow(aWindowName)
{

}

ImGuiAnimationComponent::~ImGuiAnimationComponent()
{
}

void ImGuiAnimationComponent::OnInspectorGUI()
{
	ImGui::Begin(Name(), IsOpen());

	CAnimationComponent* anim = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CAnimationComponent>();
	anim;
	



	ImGui::End();
}
