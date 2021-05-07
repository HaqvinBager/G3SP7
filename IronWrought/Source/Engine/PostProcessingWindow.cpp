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
	, myWhitePointColor(1.0f)
	, myWhitePointIntensity(10.0f)
	, myExposure(1.0f)
	, myIsReinhard(false)
	, myIsUncharted(true)
	, myIsACES(false)
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

	ImVec4 tonemapColor = { 0.1f, 0.2f, 1.0f, 1.0f };
	ImGui::TextColored(tonemapColor, "Tonemapping");

	ImGui::Text("Algorithm: ");
	ImGui::SameLine();
	
	if (ImGui::Checkbox("Reinhard", &myIsReinhard))
	{
		myIsUncharted = false;
		myIsACES = false;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Uncharted", &myIsUncharted))
	{
		myIsReinhard = false;
		myIsACES = false;

	}
	ImGui::SameLine();
	if (ImGui::Checkbox("ACES", &myIsACES))
	{
		myIsReinhard = false;
		myIsUncharted = false;
	}

	bufferData.myIsReinhard = myIsReinhard;
	bufferData.myIsUncharted = myIsUncharted;
	bufferData.myIsACES = myIsACES;

	float imguiVector4[4];
	memcpy(&imguiVector4[0], &myWhitePointColor, sizeof(Vector4));
	ImGui::ColorEdit4("White Point Color", &imguiVector4[0]);
	memcpy(&myWhitePointColor, &imguiVector4[0], sizeof(Vector4));

	bufferData.myWhitePointColor = myWhitePointColor;

	if (ImGui::SliderFloat("White Point Intensity", &myWhitePointIntensity, 0.0f, 20.0f, "%.1f"))
	{
		bufferData.myWhitePointIntensity = myWhitePointIntensity;
	}

	if (ImGui::SliderFloat("Exposure", &myExposure, -5.0f, 10.0f, "%.1f"))
	{
		bufferData.myExposure = myExposure;
	}

	ImGui::Dummy({ 0.0f, 10.0f });

	ImVec4 ssaoColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	ImGui::TextColored(ssaoColor, "SSAO");

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
