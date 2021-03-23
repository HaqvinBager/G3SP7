#include "stdafx.h"
#include "AssetCustomizationWindow.h"
#include "Engine.h"
#include "Scene.h"

#include "GameObject.h"
#include "ModelComponent.h"
#include "ModelHelperFunctions.h"

#include <filesystem>

ImGuiWindow::CAssetCustomizationWindow::CAssetCustomizationWindow(const char* aName)
	: CWindow(aName)
	, myGameObject(nullptr)
	, myShowLoadAsset(false)
	, myShowLoadCustomizationFile(false)
	, myShowSaveCustomizationFile(false)
{
	ZeroMemory(myPrimaryTint, 3);
	ZeroMemory(mySecondaryTint, 3);
	ZeroMemory(myTertiaryTint, 3);
	ZeroMemory(myAccentTint, 3);

	ZeroMemory(myJSONFileName, TintedModelVariables::JSONNameBufferSize);
}

ImGuiWindow::CAssetCustomizationWindow::~CAssetCustomizationWindow()
{
	myGameObject = nullptr;
}

void ImGuiWindow::CAssetCustomizationWindow::OnEnable()
{
	if (myGameObject)
		return;
	myGameObject = new CGameObject(98789);
	IRONWROUGHT_ACTIVE_SCENE.AddInstance(myGameObject);
}

void ImGuiWindow::CAssetCustomizationWindow::OnInspectorGUI()
{
	ImGui::Begin(Name(), Open(), ImGuiWindowFlags_MenuBar);
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Asset..", "Ctrl+O")) { myShowLoadAsset = true; }
				if (ImGui::MenuItem("Open Customization File", "Ctrl+L")) { myShowLoadCustomizationFile = true; }
				if (ImGui::MenuItem("Save Customization", "Ctrl+S")) { myShowSaveCustomizationFile = true; }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::ColorEdit3(": Color 1, Primary", myPrimaryTint);
		ImGui::ColorEdit3(": Color 2, Secondary", mySecondaryTint);
		ImGui::ColorEdit3(": Color 3, Tertiary", myTertiaryTint);
		ImGui::ColorEdit3(": Color 4, Accents", myAccentTint);
		if (myGameObject->GetComponent<CModelComponent>())
		{
			auto& model = *myGameObject->GetComponent<CModelComponent>();
			model.Tint1(Vector3(myPrimaryTint));
			model.Tint2(Vector3(mySecondaryTint));
			model.Tint3(Vector3(myTertiaryTint));
			model.Tint4(Vector3(myAccentTint));
		}

		if (myShowLoadAsset)
			LoadAsset();

		if (myShowLoadCustomizationFile)
			LoadCustomizationFile();

		if (myShowSaveCustomizationFile)
			SaveCustomizationFile();
	}
	ImGui::End();
}

void ImGuiWindow::CAssetCustomizationWindow::OnDisable()
{
	myFBXAssetPaths.clear();
	myJSONPaths.clear();
	myShowLoadAsset = false;
	myShowLoadCustomizationFile = false;
	myShowSaveCustomizationFile = false;
}

void ImGuiWindow::CAssetCustomizationWindow::LoadAsset()
{
	const std::string path = ASSETPATH("Assets/Graphics/");
	myShowLoadAsset = GetPathsByExtension(path, ".fbx", myFBXAssetPaths, !myFBXAssetPaths.empty());
	ImGui::Begin("Load FBX Asset.", &myShowLoadAsset);
	{
		ImGui::TextColored(ImVec4(1,1,1,1), path.c_str());
		ImGui::BeginChild("Scrolling");
		for (size_t i = 0; i < myFBXAssetPaths.size(); ++i)
		{
			bool selected = false;
			ImGui::Selectable(myFBXAssetPaths[i].myDisplayName.c_str(), &selected);
			if (selected)
			{
				if (!myGameObject->GetComponent<CModelComponent>())
					myGameObject->AddComponent<CModelComponent>(*myGameObject, myFBXAssetPaths[i].myPath);
				else
					myGameObject->GetComponent<CModelComponent>()->SetModel(myFBXAssetPaths[i].myPath);

				mySelectedFBX = std::move(myFBXAssetPaths[i].myPath);
				myShowLoadAsset = false;
				myFBXAssetPaths.clear();
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void ImGuiWindow::CAssetCustomizationWindow::LoadCustomizationFile()
{
	const std::string path = ASSETPATH("Assets/Graphics/TintedModels/Data/");
	myShowLoadCustomizationFile = GetPathsByExtension(path, ".json", myJSONPaths, !myJSONPaths.empty());
	ImGui::Begin("Load JSON Data.", &myShowLoadCustomizationFile);
	{
		ImGui::TextColored(ImVec4(1,1,1,1), path.c_str());
		ImGui::BeginChild("Scrolling");
		for (size_t i = 0; i < myJSONPaths.size(); ++i)
		{
			bool selected = false;
			ImGui::Selectable(myJSONPaths[i].myDisplayName.c_str(), &selected);
			if (selected)
			{
				if(myGameObject->GetComponent<CModelComponent>())
					ModelHelperFunctions::LoadTintsToModelComponent(myGameObject, myJSONPaths[i].myPath, mySelectedFBX);
				else
					ModelHelperFunctions::AddModelComponentWithTintsFromData(myGameObject, myJSONPaths[i].myPath, mySelectedFBX);

				CModelComponent* model = myGameObject->GetComponent<CModelComponent>();
				Vector4 primary		= model->Tint1();
				Vector4 secondary	= model->Tint2();
				Vector4 tertiary	= model->Tint3();
				Vector4 accents		= model->Tint4();
				myPrimaryTint[0]   = primary.x;		myPrimaryTint[1]   = primary.y;		myPrimaryTint[2]   = primary.z;
				mySecondaryTint[0] = secondary.x;	mySecondaryTint[1] = secondary.y;	mySecondaryTint[2] = secondary.z;
				myTertiaryTint[0]  = tertiary.x;	myTertiaryTint[1]  = tertiary.y;	myTertiaryTint[2]  = tertiary.z;
				myAccentTint[0]	   = accents.x;		myAccentTint[1]    = accents.y;		myAccentTint[2]    = accents.z;

				mySelectedJSON = std::move(myJSONPaths[i].myPath);
				myShowLoadCustomizationFile = false;
				myJSONPaths.clear();
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void ImGuiWindow::CAssetCustomizationWindow::SaveCustomizationFile()
{
	const std::string path = ASSETPATH("Assets/Graphics/TintedModels/Data/");
	ImGui::Begin("Save JSON Data.", &myShowSaveCustomizationFile);
	{
		ImGui::TextColored(ImVec4(1,1,1,1), path.c_str());
		ImGui::Spacing();

		ImGui::InputText("", myJSONFileName, TintedModelVariables::JSONNameBufferSize);
		if (ImGui::Button("Save"))
		{
			if (strlen(myJSONFileName) > 0)
				ModelHelperFunctions::SaveTintsFromModelComponent(myGameObject, mySelectedFBX, std::string(path + myJSONFileName + ".json"));
			else
				ModelHelperFunctions::SaveTintsFromModelComponent(myGameObject, mySelectedFBX);

			myShowSaveCustomizationFile = false;
			ZeroMemory(myJSONFileName, TintedModelVariables::JSONNameBufferSize);
		}
	}
	ImGui::End();
}

const std::string CutToFileNameOnly(const std::string& aString)
{
	size_t lastSlashIndex = aString.find_last_of("\\/");
	return aString.substr(lastSlashIndex + 1, aString.length() - lastSlashIndex + 1);
}
bool ImGuiWindow::CAssetCustomizationWindow::GetPathsByExtension(const std::string& aPath, const std::string& aFileExtesion, std::vector<SAssetInfo>& anOutVector, const bool aShouldClearVector)
{
	if (aShouldClearVector)
		anOutVector.clear();
	else
		if (!anOutVector.empty())
			return true;

	std::filesystem::path p(aPath);
	std::filesystem::recursive_directory_iterator start(p);
	std::filesystem::recursive_directory_iterator end;
	ENGINE_BOOL_POPUP(start->exists(), std::string("Directory does not exist: " + aPath).c_str());
	if (!start->exists())
		return false;

	for (auto& it = start; it != end; ++it)
	{
		if (!it->is_directory())
		{
			if (!it->path().has_extension())
				continue;
			
			if (it->path().extension().string() == aFileExtesion)
				anOutVector.push_back({ it->path().relative_path().string(), CutToFileNameOnly(it->path().relative_path().string())});
		}
	}
	return true;
}
