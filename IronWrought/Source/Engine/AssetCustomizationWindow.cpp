#include "stdafx.h"
#include "AssetCustomizationWindow.h"
#include "Engine.h"
#include "Scene.h"

#include "GameObject.h"
#include "ModelComponent.h"
#include "ModelHelperFunctions.h"

#include <filesystem>
const std::string CutToFileNameOnly(const std::string& aString)
{
	size_t lastSlashIndex = aString.find_last_of("\\/");
	return aString.substr(lastSlashIndex + 1, aString.length() - lastSlashIndex + 1);
}
const std::string CutFileEnding(const std::string& aString)
{
	size_t lastSlashIndex = aString.find_last_of(".");
	return aString.substr(0, lastSlashIndex);
}

#define COLOR_TEXT_LABEL ImVec4(1.0f, 1.0f, 1.0f, 0.75f)

ImGuiWindow::CAssetCustomizationWindow::CAssetCustomizationWindow(const char* aName)
	: CWindow(aName)
	, myGameObject(nullptr)
	, myShowLoadAsset(false)
	, myShowLoadCustomizationFile(false)
	, myShowSaveCustomizationFile(false)
	, myShowOverwriteCustomizationFile(false)
	, myReplaceFBX(true)
{
	ZeroMemory(myPrimaryTint, 3);
	ZeroMemory(mySecondaryTint, 3);
	ZeroMemory(myTertiaryTint, 3);
	ZeroMemory(myAccentTint, 3);

	ZeroMemory(myJSONFileName, AssetCustomizationWindow::JSONNameBufferSize);
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
bool g_myShowPopUp = false;
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
				if (ImGui::MenuItem("Save Customization", "Ctrl+S")) { 
					myShowSaveCustomizationFile = true; 
					if (mySelectedJSON.myDisplayName.length() > 0)
					{
						std::string fileName = CutFileEnding(mySelectedJSON.myDisplayName);
						memcpy(myJSONFileName, &fileName[0], AssetCustomizationWindow::JSONNameBufferSize);
					}
				}
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

		ImGui::Spacing();
		ImGui::TextColored(COLOR_TEXT_LABEL, "Current FBX:"); ImGui::SameLine();
		ImGui::Text(mySelectedFBX.myDisplayName.c_str());
		ImGui::Spacing();
		ImGui::TextColored(COLOR_TEXT_LABEL, "Current JSON:"); ImGui::SameLine();
		ImGui::Text(mySelectedJSON.myDisplayName.c_str());

		if (myShowLoadAsset)
			LoadAsset();

		if (myShowLoadCustomizationFile)
			LoadCustomizationFile();

		if (myShowSaveCustomizationFile)
			SaveCustomizationFile();

		if (myShowOverwriteCustomizationFile)
			SaveOverwriteFile();

		ImGui::SetWindowSize(Name(), ImVec2(0, 0));// Prohibits resize and sets size to fit all items. Magic.
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
	myShowOverwriteCustomizationFile = false;
}

void ImGuiWindow::CAssetCustomizationWindow::LoadAsset()
{
	const std::string path = ASSETPATH("Assets/Graphics/");
	myShowLoadAsset = GetPathsByExtension(path, ".fbx", myFBXAssetPaths, !myFBXAssetPaths.empty());
	ImGui::Begin("Load FBX Asset.", &myShowLoadAsset, ImGuiWindowFlags_NoCollapse);
	{
		ImGui::TextColored(COLOR_TEXT_LABEL, std::string("Folder: " + path).c_str());
		ImGui::BeginChild("Scrolling", ImVec2(250.f,400.f), false, ImGuiWindowFlags_NoScrollWithMouse);
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

				mySelectedFBX = std::move(myFBXAssetPaths[i]);
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
	ImGui::Begin("Load JSON Data.", &myShowLoadCustomizationFile, ImGuiWindowFlags_NoCollapse);
	{
		ImGui::TextColored(COLOR_TEXT_LABEL, std::string("Folder: " + path).c_str());
		ImGui::Checkbox(": Use FBX from customization file", &myReplaceFBX);
		ImGui::BeginChild("Scrolling", ImVec2(250.f,400.f), false,  ImGuiWindowFlags_NoScrollWithMouse);
		for (size_t i = 0; i < myJSONPaths.size(); ++i)
		{
			bool selected = false;
			ImGui::Selectable(myJSONPaths[i].myDisplayName.c_str(), &selected);
			if (selected)
			{
				if (myGameObject->GetComponent<CModelComponent>())
				{
					if (myReplaceFBX)
						ModelHelperFunctions::ReplaceModelAndLoadTints(myGameObject, myJSONPaths[i].myPath, mySelectedFBX.myPath);
					else 
						ModelHelperFunctions::LoadTintsToModelComponent(myGameObject, myJSONPaths[i].myPath, mySelectedFBX.myPath);
				}
				else
					ModelHelperFunctions::AddModelComponentWithTintsFromData(myGameObject, myJSONPaths[i].myPath, mySelectedFBX.myPath);

				CModelComponent* model = myGameObject->GetComponent<CModelComponent>();
				Vector4 primary		= model->Tint1();
				Vector4 secondary	= model->Tint2();
				Vector4 tertiary	= model->Tint3();
				Vector4 accents		= model->Tint4();
				myPrimaryTint[0]   = primary.x;		myPrimaryTint[1]   = primary.y;		myPrimaryTint[2]   = primary.z;
				mySecondaryTint[0] = secondary.x;	mySecondaryTint[1] = secondary.y;	mySecondaryTint[2] = secondary.z;
				myTertiaryTint[0]  = tertiary.x;	myTertiaryTint[1]  = tertiary.y;	myTertiaryTint[2]  = tertiary.z;
				myAccentTint[0]	   = accents.x;		myAccentTint[1]    = accents.y;		myAccentTint[2]    = accents.z;

				mySelectedJSON = std::move(myJSONPaths[i]);
				mySelectedFBX.myDisplayName = CutToFileNameOnly(mySelectedFBX.myPath);
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
	ImGui::Begin("Save JSON Data.", &myShowSaveCustomizationFile, ImGuiWindowFlags_NoCollapse);
	{
		ImGui::TextColored(COLOR_TEXT_LABEL, std::string("Folder: " + path).c_str());
		ImGui::Spacing();

		ImGui::InputText("", myJSONFileName, AssetCustomizationWindow::JSONNameBufferSize);
		if (ImGui::Button("Save"))
		{
			if (myGameObject->GetComponent<CModelComponent>())
			{
				if (strlen(myJSONFileName) > 0)
				{
					if (!std::filesystem::exists(std::string(path + myJSONFileName + ".json")))
					{
						SaveJSON();
						myShowSaveCustomizationFile = false;
					}
					else
					{
						Vector2 resolution = IRONWROUGHT->GetResolution() * 0.5f;
						ImGui::SetNextWindowPos(ImVec2(resolution.x - 100, resolution.y));
						ImGui::SetNextWindowFocus();
						myShowOverwriteCustomizationFile = true;
					}
				}
				else
					ModelHelperFunctions::SaveTintsFromModelComponent(myGameObject, mySelectedFBX.myPath);
			}
		}
	}
	ImGui::End();
}

void ImGuiWindow::CAssetCustomizationWindow::SaveOverwriteFile()
{
	ImGui::Begin("File Exists.", &myShowOverwriteCustomizationFile, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	{
		ImGui::SetWindowSize("File Exists.", ImVec2(200, 0));
		ImGui::TextColored(COLOR_TEXT_LABEL, "Overwrite exisitng file?");
		const float width	= ImGui::GetWindowWidth() * 0.33f;
		const float cursorX = ImGui::GetWindowWidth() * 0.49f - width;
		ImGui::SetCursorPosX(cursorX);
		
		if (ImGui::Button("Yes", ImVec2(width, 0.0f))) 
		{
			SaveJSON();
			myShowSaveCustomizationFile = false;
			myShowOverwriteCustomizationFile = false;
		} 
		ImGui::SameLine();

		if (ImGui::Button("No", ImVec2(width, 0.0f)))
			myShowOverwriteCustomizationFile = false;
	}
	ImGui::End();
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

void ImGuiWindow::CAssetCustomizationWindow::SaveJSON(const std::string& aCustomPath)
{
	std::string jsonPath;
	if (aCustomPath.length() > 0)
		jsonPath = (aCustomPath + myJSONFileName + ".json");
	else
		jsonPath = (ASSETPATH("Assets/Graphics/TintedModels/Data/") + myJSONFileName + ".json");

	ModelHelperFunctions::SaveTintsFromModelComponent(myGameObject, mySelectedFBX.myPath, jsonPath);

	mySelectedJSON.myPath = jsonPath;
	mySelectedJSON.myDisplayName = myJSONFileName;

	ZeroMemory(myJSONFileName, AssetCustomizationWindow::JSONNameBufferSize);
}
