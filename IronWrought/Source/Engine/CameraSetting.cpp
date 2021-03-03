#include "stdafx.h"
#include "CameraSetting.h"

ImGuiWindow::CCameraSetting::CCameraSetting(const char* aName)
	: CWindow(aName)
	, myCameraSpeed(2.0f)
{
}

ImGuiWindow::CCameraSetting::~CCameraSetting()
{
}

void ImGuiWindow::CCameraSetting::OnEnable()
{
}

void ImGuiWindow::CCameraSetting::OnInspectorGUI()
{
	ImGui::Begin(Name(), Open());

	//if (ImGui::BeginCombo(Name(), "Speed", ImGuiComboFlags_NoPreview)) {
		if (ImGui::SliderFloat("Camera Settings", &myCameraSpeed, 1.0f, 20.0f)) {
			//CCameraComponent* camera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
			//camera->SetSpeed(myCameraSpeed);
		}
		//ImGui::EndCombo();
	//}

	ImGui::End();
}

void ImGuiWindow::CCameraSetting::OnDisable()
{
}