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
	, myShowOpenAssetWindow(false)
	, myShowOpenCustomizationWindow(false)
	, myShowSaveCustomizationWindow(false)
{
	ZeroMemory(myPrimaryTint, 3);
	ZeroMemory(mySecondaryTint, 3);
	ZeroMemory(myTertiaryTint, 3);
	ZeroMemory(myAccentTintTint, 3);
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
				if (ImGui::MenuItem("Open Asset..", "Ctrl+O")) { myShowOpenAssetWindow = true; }
				if (ImGui::MenuItem("Open Customization File", "Ctrl+L")) { OpenCustomizationFile(); }
				if (ImGui::MenuItem("Save Customization", "Ctrl+S")) { SaveCustomizationFile(); }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::ColorEdit3(": Color 1, Primary", myPrimaryTint);
		ImGui::ColorEdit3(": Color 2, Secondary", mySecondaryTint);
		ImGui::ColorEdit3(": Color 3, Tertiary", myTertiaryTint);
		ImGui::ColorEdit3(": Color 4, Accents", myAccentTintTint);
		if (myGameObject->GetComponent<CModelComponent>())
		{
			auto& model = *myGameObject->GetComponent<CModelComponent>();
			model.Tint1(Vector3(myPrimaryTint));
			model.Tint2(Vector3(mySecondaryTint));
			model.Tint3(Vector3(myTertiaryTint));
			model.Tint4(Vector3(myAccentTintTint));
		}

		if (myShowOpenAssetWindow)
			OpenAsset();
	}
	ImGui::End();
}

void ImGuiWindow::CAssetCustomizationWindow::OnDisable()
{}

void ImGuiWindow::CAssetCustomizationWindow::OpenAsset()
{
	// Open ImGui window with browser for Assets/
	// Double click lets you load FBX
	// Finds textures
	myShowOpenAssetWindow = GetPathsByExtension(ASSETPATH("Assets/Graphics/"), ".fbx", myFBXAssetPaths, !myFBXAssetPaths.empty());
	ImGui::Begin("Open FBX Asset", &myShowOpenAssetWindow);
	{
		ImGui::TextColored(ImVec4(1,1,1,1), "FBXses");
		ImGui::BeginChild("Scrolling");
		for (size_t i = 0; i < myFBXAssetPaths.size(); ++i)
		{
			bool selected = false;
			ImGui::Selectable(myFBXAssetPaths[i].myDisplayName.c_str(), &selected);
			if (selected)
			{
				if (!myGameObject->GetComponent<CModelComponent>())
				{
					myGameObject->AddComponent<CModelComponent>(*myGameObject, myFBXAssetPaths[i].myPath);
				}
				else
				{
					myGameObject->GetComponent<CModelComponent>()->SetModel(myFBXAssetPaths[i].myPath);
				}
				myShowOpenAssetWindow = false;
			}

		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void ImGuiWindow::CAssetCustomizationWindow::OpenCustomizationFile()
{
	// Open ImGui with browser for TintedModelData/
	// Double click lets you load data from selected json.
}

void ImGuiWindow::CAssetCustomizationWindow::SaveCustomizationFile()
{
	// ImGui window where you can enter the name of the file.
	// Saves colors and model to name_of_file.json, in TintedModelData/
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
