#include "stdafx.h"
#include "PostProcessingWindow.h"
#include "FullscreenRenderer.h"
#include "Engine.h"
#include <imgui.h>

IronWroughtImGui::CPostProcessingWindow::CPostProcessingWindow(const char* aName)
	: CWindow(aName)
	, mySSAORadius(0.6f)
	, mySSAOSampleBias(0.005f)
	, mySSAOMagnitude(1.1f)
	, mySSAOContrast(1.5f)
	, mySSAOConstantBias(0.2f)
{
}

IronWroughtImGui::CPostProcessingWindow::~CPostProcessingWindow()
{
}

void IronWroughtImGui::CPostProcessingWindow::OnEnable()
{
}

void IronWroughtImGui::CPostProcessingWindow::OnInspectorGUI()
{
	CFullscreenRenderer::SPostProcessingBufferData bufferData = IRONWROUGHT->GetPostProcessingBufferData();

	ImGui::Begin(Name(), Open());

	ImVec4 color = { 1.0f, 0.0f, 0.0f, 1.0f };
	ImGui::TextColored(color, "SSAO");

	if (ImGui::SliderFloat("Sample Kernel Radius", &mySSAORadius, 0.1f, 10.0f, "%.1f"))
	{
		bufferData.mySSAORadius = mySSAORadius;	
	}

	if (ImGui::SliderFloat("Sample Bias", &mySSAOSampleBias, 0.0001f, 1.0f, "%.4f", ImGuiSliderFlags_Logarithmic))
	{
		bufferData.mySSAOSampleBias = mySSAOSampleBias;
	}

	if (ImGui::SliderFloat("Magnitude", &mySSAOMagnitude, 0.1f, 5.0f, "%.1f", ImGuiSliderFlags_Logarithmic))
	{
		bufferData.mySSAOMagnitude = mySSAOMagnitude;
	}

	if (ImGui::SliderFloat("Contrast", &mySSAOContrast, 0.1f, 5.0f, "%.1f", ImGuiSliderFlags_Logarithmic))
	{
		bufferData.mySSAOContrast = mySSAOContrast;
	}

	ImGui::End();

	IRONWROUGHT->SetPostProcessingBufferData(bufferData);
}

void IronWroughtImGui::CPostProcessingWindow::OnDisable()
{
}
