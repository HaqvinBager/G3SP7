#include "stdafx.h"
#include "EnvironmentLightWindow.h"
#include "EnvironmentLight.h"
#include "EnviromentLightComponent.h"
#include "Engine.h"
#include "Scene.h"
#include <imgui.h>

IronWroughtImGui::CEnvironmentLightWindow::CEnvironmentLightWindow(const char* aName)
	: CWindow(aName)
	, myColor(Vector3::One)
	, myNumberOfSamples(16.0f)
	, myLightPower(100000.0f)
	, myScatteringProbability(0.001f)
	, myHenyeyGreensteinGValue(0.0f)
{
	if (&CEngine::GetInstance()->GetActiveScene())
	{
		CEnvironmentLight* light = CEngine::GetInstance()->GetActiveScene().EnvironmentLight();
		if (light)
		{
			myIsVolumetric = light->GetIsVolumetric();
			myIsFog = light->GetIsFog();
			myNumberOfSamples = light->GetNumberOfSamples();
			myLightPower = light->GetLightPower();
			myScatteringProbability = light->GetScatteringProbability();
			myHenyeyGreensteinGValue = light->GetHenyeyGreensteinGValue();
		}
	}

}

IronWroughtImGui::CEnvironmentLightWindow::~CEnvironmentLightWindow()
{
}

void IronWroughtImGui::CEnvironmentLightWindow::OnEnable()
{
}

void IronWroughtImGui::CEnvironmentLightWindow::OnInspectorGUI()
{
	ImGui::Begin(Name(), Open());

	CEnvironmentLight* light = CEngine::GetInstance()->GetActiveScene().EnvironmentLight();

	float imguiVector3[3];
	memcpy(&imguiVector3[0], &myColor, sizeof(Vector3));
	ImGui::ColorEdit3("Start Color", &imguiVector3[0]);
	memcpy(&myColor, &imguiVector3[0], sizeof(Vector3));
	light->SetColor(myColor);

	ImGui::Dummy({ 0.0f, 10.0f });

	if (ImGui::Checkbox("Is Volumetric", &myIsVolumetric))
	{
		light->SetIsVolumetric(myIsVolumetric);
	}

	if (ImGui::Checkbox("Is Fog", &myIsFog))
	{
		light->SetIsFog(myIsFog);
	}

	if (ImGui::SliderFloat("Number of Samples", &myNumberOfSamples, 8.0f, 128.0f, "%.0f")) 
	{
		light->SetNumberOfSamples(myNumberOfSamples);
	}

	if (ImGui::SliderFloat("Light Power", &myLightPower, 1000.0f, 10000000.0f, "%.0f", ImGuiSliderFlags_Logarithmic)) 
	{
		light->SetLightPower(myLightPower);
	}

	if (ImGui::SliderFloat("Scattering Probability", &myScatteringProbability, 0.00001f, 0.1f, "%.5f", ImGuiSliderFlags_Logarithmic)) 
	{
		light->SetScatteringProbability(myScatteringProbability);
	}

	if (ImGui::SliderFloat("Henyey-Greenstein G-Value", &myHenyeyGreensteinGValue, -1.0f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic)) 
	{
		light->SetHenyeyGreensteinGValue(myHenyeyGreensteinGValue);
	}

	ImGui::End();
}

void IronWroughtImGui::CEnvironmentLightWindow::OnDisable()
{
}
