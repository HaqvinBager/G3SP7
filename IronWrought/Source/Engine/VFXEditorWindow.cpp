#include "stdafx.h"
#include "VFXEditorWindow.h"
#include <widget_curve.h>
#include <curve_v122.hpp>
#include "JsonReader.h"
#include "JsonWriter.h"

#include "Engine.h"
#include "Scene.h"
#include "VFXSystemComponent.h"

#include "rapidjson/filewritestream.h"
#include <fstream>

#define CURRENT_EFFECT myEffects[myCurrentEffectIndex]

IronWroughtImGui::CVFXEditorWindow::CVFXEditorWindow(const char* aName)
	: CWindow(aName)
{
	myPointsMap["First Curve"] = { {1.0f, 1.0f}, {0.25f, 0.25f}, {0.75f, 0.75f}, {1.0f, 1.0f} };

	std::string saveDestination = "Assets/Graphics/VFX/JSON/VFXSystemTester.json";
	ZeroMemory(mySaveDestination, 256);
	memcpy(&mySaveDestination[0], saveDestination.c_str(), strlen(saveDestination.c_str()));

	AddEffect(0);

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
IronWroughtImGui::CVFXEditorWindow::~CVFXEditorWindow()
{
}

void IronWroughtImGui::CVFXEditorWindow::OnEnable()
{
}

void IronWroughtImGui::CVFXEditorWindow::OnInspectorGUI()
{
	ImGui::Begin(Name(), Open());
	//for (auto& keyValue : myPointsMap)
	//{
	//	//std::vector<ImVec2> vec = keyValue.second;
	//	//ImGui::IronCurve(keyValue.first.c_str(), keyValue.second.data());
	//	
	//}

	if (!myMeshData && !myEmitterData)
	{
		ShowMainMenu();
	}
	else if (myMeshData)
	{
		ShowVFXMeshWindow();
	}
	else if (myEmitterData)
	{
		ShowParticleEffectWindow();
	}

	ImGui::End();
}

void IronWroughtImGui::CVFXEditorWindow::OnDisable()
{
}

void IronWroughtImGui::CVFXEditorWindow::SaveToFile()
{
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
	writer.SetIndent(' ', 2);

	writer.StartObject();
	writer.Key("VFXEffects");
	writer.StartArray();

	for (auto& effect : myEffects)
	{
		effect.SerializeEffects(writer);
	}

	writer.EndArray();
	writer.EndObject();

	std::ofstream of(ASSETPATH(mySaveDestination).c_str());
	of << s.GetString();

	of.close();

	CGameObject* vfx = IRONWROUGHT->GetActiveScene().GetVFXTester();
	vfx->GetComponent<CVFXSystemComponent>()->Init(ASSETPATH(mySaveDestination));
	vfx->GetComponent<CVFXSystemComponent>()->EnableEffect(0);
}

void IronWroughtImGui::CVFXEditorWindow::ShowMainMenu()
{
	for (unsigned int i = 0; i < myEffects.size(); ++i)
	{
		ImGui::SameLine(0.0f, 20.0f);
		if (ImGui::Button(myEffects[i].myName))
		{
			myCurrentEffectIndex = i;
		}
	}
	ImGui::SameLine(0.0f, 20.f);
	if (ImGui::Button("Add Effect"))
	{
		AddEffect(static_cast<unsigned int>(myEffects.size()));
	}

	if (myEffects.empty())
	{
		ImGui::End();
		return;
	}

	ImGui::InputText("Effect Name", &CURRENT_EFFECT.myName[0], 64);
	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "VFX Meshes");
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button("Add VFX Mesh"))
	{
		SVFXMeshTransformData meshData;
		CURRENT_EFFECT.myVFXMeshes.push_back(meshData);
	}
	for (unsigned int i = 0; i < CURRENT_EFFECT.myVFXMeshes.size(); ++i)
	{
		ShowVFXMeshTransformData(CURRENT_EFFECT.myVFXMeshes[i], i);
	}
	ImGui::Spacing();

	ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "ParticleSystems");
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button("Add Particle Emitter"))
	{
		SParticleEmitterTransformData emitterData;
		CURRENT_EFFECT.myParticleEmitters.push_back(emitterData);
	}
	for (unsigned int i = 0; i < CURRENT_EFFECT.myParticleEmitters.size(); ++i)
	{
		ShowParticleEmitterTransformData(CURRENT_EFFECT.myParticleEmitters[i], i);
	}
	ImGui::Spacing();

	ImGui::InputText("Save As", mySaveDestination, 256);
	if (ImGui::Button("Save")) {
		SaveToFile();
	}
}

void IronWroughtImGui::CVFXEditorWindow::AddEffect(unsigned int anIndex)
{
	SVFXSerializable serializable;
	std::string name = "Effect " + std::to_string(anIndex);
	ZeroMemory(serializable.myName, 64);
	memcpy(&serializable.myName[0], name.c_str(), strlen(name.c_str()));

	SVFXMeshTransformData meshData;
	serializable.myVFXMeshes.push_back(meshData);

	SParticleEmitterTransformData emitterData;
	serializable.myParticleEmitters.push_back(emitterData);

	myEffects.push_back(serializable);
}

void IronWroughtImGui::CVFXEditorWindow::ShowVFXMeshTransformData(SVFXMeshTransformData& someData, unsigned int anIndex)
{
	std::string index = std::to_string(anIndex);

	std::string label = "VM Path " + index;
	ImGui::InputText(label.c_str(), &someData.myPath[0], 128);

	label = "VM Modify " + index;
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button(label.c_str()))
	{
		OpenVFXMeshWindow(someData.myPath);
	}

	float imguiVector[3];
	label = "VM Offset From Parent " + index;
	memcpy(&imguiVector[0], &someData.myOffsetFromParent, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.2f");
	memcpy(&someData.myOffsetFromParent, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "VM Rotation Around Parent " + index;
	memcpy(&imguiVector[0], &someData.myRotationAroundParent, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myRotationAroundParent, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "VM Angular Speeds " + index;
	memcpy(&imguiVector[0], &someData.myAngularSpeeds, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myAngularSpeeds, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "VM Scale " + index;
	memcpy(&imguiVector[0], &someData.myScale, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myScale, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "VM Orbit Around Parent " + index;
	ImGui::Checkbox(label.c_str(), &someData.myShouldOrbit);

	label = "VM Delay " + index;
	ImGui::DragFloat(label.c_str(), &someData.myDelay, 0.1f, 0.0f, 10.0f, "%.1f");

	label = "VM Duration " + index;
	ImGui::DragFloat(label.c_str(), &someData.myDuration, 0.2f, 0.1f, 10.0f, "%.1f");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
}

void IronWroughtImGui::CVFXEditorWindow::ShowParticleEmitterTransformData(SParticleEmitterTransformData& someData, unsigned int anIndex)
{
	std::string index = std::to_string(anIndex);

	std::string label = "PS Path " + index;
	ImGui::InputText(label.c_str(), &someData.myPath[0], 128);

	label = "PS Modify " + index;
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button(label.c_str()))
	{
		OpenParticleEffectWindow(someData.myPath);
	}

	float imguiVector[3];
	label = "PS Offset From Parent " + index;
	memcpy(&imguiVector[0], &someData.myOffsetFromParent, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.2f");
	memcpy(&someData.myOffsetFromParent, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "PS Rotation Around Parent " + index;
	memcpy(&imguiVector[0], &someData.myRotationAroundParent, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myRotationAroundParent, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "PS Angular Speeds " + index;
	memcpy(&imguiVector[0], &someData.myAngularSpeeds, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myAngularSpeeds, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "PS Uniform Scale " + index;
	ImGui::DragFloat(label.c_str(), &someData.myUniformScale, 0.01f, 0.01f, 10.0f, "%.2f");

	label = "PS Orbit Around Parent " + index;
	ImGui::Checkbox(label.c_str(), &someData.myShouldOrbit);

	label = "PS Delay " + index;
	ImGui::DragFloat(label.c_str(), &someData.myDelay, 0.1f, 0.0f, 10.0f, "%.1f");

	label = "PS Duration " + index;
	ImGui::DragFloat(label.c_str(), &someData.myDuration, 0.2f, 0.1f, 10.0f, "%.1f");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
}

void IronWroughtImGui::CVFXEditorWindow::ShowVFXMeshWindow()
{
	ImGui::TextColored({ 1.0f, 0.0f, 1.0f, 1.0f }, "VFX Mesh File");
	ImGui::Spacing();

	float imguiVector[2];
	memcpy(&imguiVector[0], &myMeshData->myScrollSpeed1, sizeof(DirectX::SimpleMath::Vector2));
	ImGui::InputFloat2("Scroll Speed Texture 1", &imguiVector[0], "%.2f");
	memcpy(&myMeshData->myScrollSpeed1, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector2));

	memcpy(&imguiVector[0], &myMeshData->myScrollSpeed2, sizeof(DirectX::SimpleMath::Vector2));
	ImGui::InputFloat2("Scroll Speed Texture 2", &imguiVector[0], "%.2f");
	memcpy(&myMeshData->myScrollSpeed2, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector2));

	memcpy(&imguiVector[0], &myMeshData->myScrollSpeed3, sizeof(DirectX::SimpleMath::Vector2));
	ImGui::InputFloat2("Scroll Speed Texture 3", &imguiVector[0], "%.2f");
	memcpy(&myMeshData->myScrollSpeed3, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector2));

	memcpy(&imguiVector[0], &myMeshData->myScrollSpeed4, sizeof(DirectX::SimpleMath::Vector2));
	ImGui::InputFloat2("Scroll Speed Texture 4", &imguiVector[0], "%.2f");
	memcpy(&myMeshData->myScrollSpeed4, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector2));

	ImGui::DragFloat("UV Scale Texture 1", &myMeshData->myUVScale1, 0.1f, 0.1f, 10.0f, "%.2f");
	ImGui::DragFloat("UV Scale Texture 2", &myMeshData->myUVScale2, 0.1f, 0.1f, 10.0f, "%.2f");
	ImGui::DragFloat("UV Scale Texture 3", &myMeshData->myUVScale3, 0.1f, 0.1f, 10.0f, "%.2f");
	ImGui::DragFloat("UV Scale Texture 4", &myMeshData->myUVScale4, 0.1f, 0.1f, 10.0f, "%.2f");

	ImGui::DragFloat("Opacity Strength", &myMeshData->myOpacityStrength, 0.1f, 0.1f, 10.0f, "%.2f");

	ImGui::InputText("Texture 1 Path", &myMeshData->myTexturePath1[0], 64);
	ImGui::InputText("Texture 2 Path", &myMeshData->myTexturePath2[0], 64);
	ImGui::InputText("Texture 3 Path", &myMeshData->myTexturePath3[0], 64);
	ImGui::InputText("Texture Mask Path", &myMeshData->myTextureMaskPath[0], 64);
	ImGui::InputText("Mesh Path", &myMeshData->myMeshPath[0], 64);

	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	ImGui::InputText("Save As", myVFXMeshDataDestination, 256);
	if (ImGui::Button("Save"))
	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
		writer.SetIndent(' ', 2);

		myMeshData->Serialize(writer);

		std::ofstream of(ASSETPATH(myVFXMeshDataDestination));
		of << s.GetString();

		of.close();

		CGameObject* vfx = IRONWROUGHT->GetActiveScene().GetVFXTester();
		vfx->GetComponent<CVFXSystemComponent>()->Init(ASSETPATH(mySaveDestination));
		vfx->GetComponent<CVFXSystemComponent>()->EnableEffect(0);
	}
	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	if (ImGui::Button("Back to VFX Effect"))
	{
		delete myMeshData;
		myMeshData = nullptr;
	}
}

void IronWroughtImGui::CVFXEditorWindow::ShowParticleEffectWindow()
{
	ImGui::TextColored({ 1.0f, 0.0f, 1.0f, 1.0f }, "Particle Emitter File");
	ImGui::Spacing();

	if (ImGui::Button("Save"))
	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
		writer.SetIndent(' ', 2);

		myEmitterData->Serialize(writer);

		std::ofstream of(ASSETPATH(myParticleEmitterDataDestination));
		of << s.GetString();

		of.close();

		CGameObject* vfx = IRONWROUGHT->GetActiveScene().GetVFXTester();
		vfx->GetComponent<CVFXSystemComponent>()->Init(ASSETPATH(mySaveDestination));
		vfx->GetComponent<CVFXSystemComponent>()->EnableEffect(0);
	}
}

void IronWroughtImGui::CVFXEditorWindow::OpenVFXMeshWindow(const std::string& aPath)
{
	if (myMeshData)
	{
		delete myMeshData;
		myMeshData = nullptr;
	}

	myMeshData = new SVFXMeshData;
	myMeshData->Init(aPath);

	ZeroMemory(myVFXMeshDataDestination, 256);
	memcpy(&myVFXMeshDataDestination[0], aPath.c_str(), strlen(aPath.c_str()));
}

void IronWroughtImGui::CVFXEditorWindow::OpenParticleEffectWindow(const std::string& aPath)
{
	if (myEmitterData)
	{
		delete myEmitterData;
		myEmitterData = nullptr;
	}

	myEmitterData = new SParticleEmitterData;
	myEmitterData->Init(aPath);

	ZeroMemory(myParticleEmitterDataDestination, 256);
	memcpy(&myParticleEmitterDataDestination[0], aPath.c_str(), strlen(aPath.c_str()));
}
