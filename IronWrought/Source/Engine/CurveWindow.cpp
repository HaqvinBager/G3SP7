#include "stdafx.h"
#include "CurveWindow.h"
#include <widget_curve.h>
#include <curve_v122.hpp>
#include "JsonReader.h"
#include "JsonWriter.h"

#include "Engine.h"
#include "Scene.h"
#include "VFXSystemComponent.h"

#include "rapidjson/filewritestream.h"
#include <fstream>

IronWroughtImGui::CCurveWindow::CCurveWindow(const char* aName)
	: CWindow(aName)	
{
	myPointsMap["First Curve"] = { {1.0f, 1.0f}, {0.25f, 0.25f}, {0.75f, 0.75f}, {1.0f, 1.0f} };

	SVFXSerializable serializable;
	std::string name = "Pull\0";
	ZeroMemory(serializable.myName, 64);
	memcpy(&serializable.myName[0], name.c_str(), strlen(name.c_str()));
	
	//char path[128] = "Assets/Graphics/VFX/JSON/Data/ParticleData_Pull.json\0";
	//std::string stringpath = "Assets/Graphics/VFX/JSON/Data/ParticleData_Pull.json\0";
	//memcpy(&path[0], stringpath.c_str(), strlen(stringpath.c_str()));

	//serializable.myParticleEmitterPaths.push_back(path);
	////ZeroMemory(serializable.myParticleEmitterPaths[0], 128);

	serializable.myEmitterParentOffsets.push_back({ 0.0f, 0.0f, 0.0f });
	serializable.myEmitterRotationsAroundParent.push_back({ 0.0f, 0.0f, 0.0f });
	serializable.myEmitterScale.push_back(1.0f);
	serializable.myEmitterAngularSpeeds.push_back({ 0.0f, 0.0f, 0.0f });
	serializable.myEmitterShouldOrbit.push_back(true);
	serializable.myEmitterBaseDelays.push_back(0.0f);
	serializable.myEmitterDurations.push_back(5.0f);
	serializable.myParticleSizeCurves.push_back(myPointsMap["First Curve"]);
	myEffects.push_back(serializable);



	//SVFXSerializable serializable2;
	//name = "Push\0";
	//ZeroMemory(serializable2.myName, 64);
	//memcpy(&serializable2.myName[0], name.c_str(), strlen(name.c_str()));

	////char path2[128];
	////stringpath = "Assets/Graphics/VFX/JSON/Data/ParticleData_Pull.json\0";
	////memcpy(&path2[0], stringpath.c_str(), strlen(stringpath.c_str()));
	////serializable.myParticleEmitterPaths.push_back(path2);
	//////ZeroMemory(serializable.myParticleEmitterPaths[0], 128);
	//serializable2.myEmitterParentOffsets.push_back({ 0.0f, 0.0f, 0.0f });
	//serializable2.myEmitterRotationsAroundParent.push_back({ 0.0f, 0.0f, 0.0f });
	//serializable2.myEmitterScale.push_back(1.0f);
	//serializable2.myEmitterAngularSpeeds.push_back({ 0.0f, 0.0f, 0.0f });
	//serializable2.myParticleSizeCurves.push_back(myPointsMap["First Curve"]);
	//myEffects.push_back(serializable2);

/* 1hr */	//Read all VFX Json Files 
/* 1hr */		//Drop-Down Selector for choosing which VFX you want to edit
/* ^^^ */		//Load Selected Data into memory
/* 4hr */			//Visualize the Data for each different Type
						//Vector3
						//Vector2
						//Bezier Curve (For any single-float values?)
						//Select Other Json file w/ specific Prefix (_PD or something)?
						//Names for all the values
			
/* 1hr */		//Save Edited Data back into Json File
/* 2hr */			//Use a temp-json file that saves the current state of the data
/* 1hr */			//If the user closes the window, this temp file writes to the VFX Json (aka it saves whatever u did):))<3

}
IronWroughtImGui::CCurveWindow::~CCurveWindow()
{
}

void IronWroughtImGui::CCurveWindow::OnEnable()
{
}

void IronWroughtImGui::CCurveWindow::OnInspectorGUI()
{
	ImGui::Begin(Name(), Open());
	//for (auto& keyValue : myPointsMap)
	//{
	//	//std::vector<ImVec2> vec = keyValue.second;
	//	//ImGui::IronCurve(keyValue.first.c_str(), keyValue.second.data());
	//	
	//}
	for (unsigned int i = 0; i < myEffects.size(); ++i)
	{
		ImGui::SameLine(0.0f, 20.0f);
		if (ImGui::Button(myEffects[i].myName))
		{
			myCurrentEffectIndex = i;
		}
	}

	//switch (myCurrentEffectIndex)
	//{
	//case 1:
	//	ImGui::ColorEdit4("Color", myColor);
	//	break;
	//}
	ImGui::InputText("Effect Name", &myEffects[myCurrentEffectIndex].myName[0], 64);
	ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "ParticleSystems");

	//ImGui::InputText("Path", &myEffects[myCurrentEffectIndex].myParticleEmitterPaths[0][0], 128);
	ImGui::DragFloat("Offset From Parent X", &myEffects[myCurrentEffectIndex].myEmitterParentOffsets[0].x, 0.25f, -10.0f, 10.0f, "%.2f");
	ImGui::DragFloat("Offset From Parent Y", &myEffects[myCurrentEffectIndex].myEmitterParentOffsets[0].y, 0.25f, -10.0f, 10.0f, "%.2f");
	ImGui::DragFloat("Offset From Parent Z", &myEffects[myCurrentEffectIndex].myEmitterParentOffsets[0].z, 0.25f, -10.0f, 10.0f, "%.2f");

	ImGui::DragFloat("Rotation Around Parent X", &myEffects[myCurrentEffectIndex].myEmitterRotationsAroundParent[0].x, 1.0f, -180.0f, 180.0f, "%.0f");
	ImGui::DragFloat("Rotation Around Parent Y", &myEffects[myCurrentEffectIndex].myEmitterRotationsAroundParent[0].y, 1.0f, -180.0f, 180.0f, "%.0f");
	ImGui::DragFloat("Rotation Around Parent Z", &myEffects[myCurrentEffectIndex].myEmitterRotationsAroundParent[0].z, 1.0f, -180.0f, 180.0f, "%.0f");

	ImGui::DragFloat("Uniform Scale", &myEffects[myCurrentEffectIndex].myEmitterScale[0], 0.01f, 0.01f, 10.0f, "%.2f");

	ImGui::DragFloat("Rotation Per Second X", &myEffects[myCurrentEffectIndex].myEmitterAngularSpeeds[0].x, 1.0f, -360.0f, 360.0f, "%.0f");
	ImGui::DragFloat("Rotation Per Second Y", &myEffects[myCurrentEffectIndex].myEmitterAngularSpeeds[0].y, 1.0f, -360.0f, 360.0f, "%.0f");
	ImGui::DragFloat("Rotation Per Second Z", &myEffects[myCurrentEffectIndex].myEmitterAngularSpeeds[0].z, 1.0f, -360.0f, 360.0f, "%.0f");

	ImGui::DragFloat("Delay", &myEffects[myCurrentEffectIndex].myEmitterBaseDelays[0], 0.1f, 0.0f, 10.0f, "%.1f");
	
	ImGui::DragFloat("Duration", &myEffects[myCurrentEffectIndex].myEmitterDurations[0], 0.2f, 0.1f, 10.0f, "%.1f");
	//auto bools = std::vector<bool>::reference(myEffects[myCurrentEffectIndex].myVFXShouldOrbit);
	//ImGui::Checkbox("Should Orbit", bools);
	////ImGui::InputFloat3("Offset From Parent", &myEffects[myCurrentEffectIndex].myEmitterParentOffsets[myCurrentEffectIndex][0])


	//ImGui::DragFloat3("Offset From Parent",)


	if (ImGui::Button("Save")) {
		SaveToFile();
	}
	ImGui::End();
}

void IronWroughtImGui::CCurveWindow::OnDisable()
{
}

void IronWroughtImGui::CCurveWindow::SaveToFile()
{
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
	writer.SetIndent(' ', 2);
	
	writer.StartObject();
	writer.Key("VFXEffects");
	writer.StartArray();

	myEffects[myCurrentEffectIndex].Serialize(writer);
	
	writer.EndArray();
	writer.EndObject();
	
	std::ofstream of(ASSETPATH("Assets/Graphics/VFX/JSON/VFXSystemTester.json").c_str());
	of << s.GetString();

	of.close();

	CGameObject* vfx = IRONWROUGHT->GetActiveScene().GetVFXTester();
	vfx->GetComponent<CVFXSystemComponent>()->Init(ASSETPATH("Assets/Graphics/VFX/JSON/VFXSystemTester.json"));
	vfx->GetComponent<CVFXSystemComponent>()->EnableEffect(0);

	//CJsonWriter::Get()->WriteGenericValue(ASSETPATH("Assets/Graphics/VFX/JSON/VFXSystemTester.json").c_str(), {"VFXEffects", "0", "ParticleSystems", "0", "Offset X"}, 0.1f);
}

//void IronWroughtImGui::CCurveWindow::Save(const char* aJsonFile, const char* aVariableName, const std::vector<ImVec2>& somePoints)
//{
//
//	//CJsonWriter::Get()->WriteGenericValue(aJsonFile, )
//}
