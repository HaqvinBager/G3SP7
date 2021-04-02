#include "stdafx.h"
#include "GravityGloveEditor.h"
#include "Engine.h"
#include "Scene.h"
#include <GravityGloveComponent.h>

IronWroughtImGui::CGravityGloveEditor::CGravityGloveEditor(const char* aName)
	: CWindow(aName)
{
}

IronWroughtImGui::CGravityGloveEditor::~CGravityGloveEditor()
{
}

void IronWroughtImGui::CGravityGloveEditor::OnEnable()
{
	myGlove = IRONWROUGHT->GetActiveScene().FindFirstObjectWithComponent<CGravityGloveComponent>();

	if (myGlove != nullptr)
	{
		//myValues["Push Force"] = &myGlove->mySettings.myPushForce;// serializedData.myPushForce;
		//myValues["Max Push Force"] = &myGlove->mySettings.myMaxPushForce;
		//myValues["Minimum Velocity On Arrive"] = &myGlove->mySettings.myDistanceToMaxLinearVelocity;
		myValues["Max Distance"] = &myGlove->mySettings.myMaxDistance;
		myValues["Min Pull Force"] = &myGlove->mySettings.myMinPullForce;
		myValues["Max Pull Force"] = &myGlove->mySettings.myMaxPullForce;
		myValues["Current Distance Procent"] = &myGlove->mySettings.myCurrentDistanceInverseLerp;


	}
}

void IronWroughtImGui::CGravityGloveEditor::OnInspectorGUI()
{
	if (myGlove == nullptr)
		*Open() = false;

	ImGui::Begin(Name(), Open());

	for (auto& value : myValues)
	{
		ImGui::DragFloat(value.first, value.second, 0.1f, 0.0f, 100.0f);
	}

	//myGlove->SetSettings(mySettings);
	ImGui::End();

}

void IronWroughtImGui::CGravityGloveEditor::OnDisable()
{
}
