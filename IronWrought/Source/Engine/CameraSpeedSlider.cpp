#include "stdafx.h"
#include "CameraSpeedSlider.h"

ImGuiWindow::CCameraSpeedSlider::CCameraSpeedSlider(const char* aName)
	: CWindow(aName)
	, myCameraSpeed(2.0f)
{
}

ImGuiWindow::CCameraSpeedSlider::~CCameraSpeedSlider()
{
}

void ImGuiWindow::CCameraSpeedSlider::OnEnable()
{
}

void ImGuiWindow::CCameraSpeedSlider::OnInspectorGUI()
{
	ImGui::Begin(Name(), Open());

	//if (ImGui::BeginCombo(Name(), "Speed", ImGuiComboFlags_NoPreview)) {
		if (ImGui::SliderFloat("Camera Speed", &myCameraSpeed, 1.0f, 20.0f)) {
			//CCameraComponent* camera = scene->FindFirstObjectWithComponent<CCameraComponent>();
			//camera->SetSpeed(myCameraSpeed);
		}
		//ImGui::EndCombo();
	//}

	ImGui::End();
}

void ImGuiWindow::CCameraSpeedSlider::OnDisable()
{
}