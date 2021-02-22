#include "stdafx.h"
#include "LoadScene.h"

#include "JsonReader.h"
#include "Scene.h"
#include "Engine.h"
#include "SceneManager.h"
#include "CameraComponent.h"

ImGuiWindow::CLoadScene::CLoadScene(const char* aMenuName, const bool aIsMenuChild)
	: CWindow(aMenuName, aIsMenuChild)
	, myState(EState::DropDownMenu)
	, mySceneIndex(-1)
{
	OnEnable();
}

ImGuiWindow::CLoadScene::~CLoadScene()
{
}

void ImGuiWindow::CLoadScene::OnEnable()
{
	std::vector<std::string> generatedJsonFiles = CJsonReader::Get()->GetFilePathsInFolder(ASSETPATH + "Assets/Generated", ".json");
	for (unsigned int i = 0; i < generatedJsonFiles.size(); ++i) {
		const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH + "Assets/Generated/" + generatedJsonFiles[i]);
		if (!doc.HasParseError()) {
			if (doc.HasMember("instancedGameobjects") &&
				doc.HasMember("modelGameObjects")) {
				myScenes.push_back(generatedJsonFiles[i]);
			}
		}
	}

	myScenes.push_back("Empty");

	myState = EState::DropDownMenu;
}


bool ImGuiWindow::CLoadScene::OnMainMenuGUI()
{
	std::string previewName;
	if (mySceneIndex < 0)
		previewName = "Empty";
	else
		previewName = myScenes[mySceneIndex];

	if (ImGui::BeginCombo(Name(), previewName.c_str(), ImGuiComboFlags_NoPreview)) {
		int index = 0;
		for (const auto& scene : myScenes) {
			if (ImGui::Selectable(scene.c_str(), mySceneIndex == index)) {

				CCameraComponent* camera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
				Vector3 camPos = camera->GameObject().myTransform->Position();

				CScene* newScene;
				if (scene == "Empty")
					newScene = CSceneManager::CreateEmpty();
				else 
					newScene = CSceneManager::CreateScene(scene);
				
				CEngine::GetInstance()->AddScene(CStateStack::EState::InGame, newScene);
				CEngine::GetInstance()->SetActiveScene(CStateStack::EState::InGame);

				CCameraComponent* newCamera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
				newCamera->GameObject().myTransform->Position(camPos);
			}
			index++;
		}
		ImGui::EndCombo();
	}
	return true;
}

void ImGuiWindow::CLoadScene::OnInspectorGUI()
{
}

void ImGuiWindow::CLoadScene::OnDisable()
{
}


/*
//std::vector<std::string> files = CJsonReader::GetFilePathsInFolder(ASSETPATH + "Assets/Generated");
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 200, 18));
	float x = 10.f + ((ImGui::GetFontSize() + 5.5f) * static_cast<float>(myLevelsToSelectFrom.size()));
	ImGui::SetNextWindowSize({200.f,  x});

	ImGui::Begin("LevelSelect", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	/*if (ImGui::TreeNodeEx("Selection State: Single Selection", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf))
	{
static int selected = -1;
for (int n = 0; n < myLevelsToSelectFrom.size(); n++)
{
	char buf[512];
	sprintf_s(buf, "%s", myLevelsToSelectFrom[n].c_str());

	if (ImGui::Selectable(buf, selected == n, ImGuiSelectableFlags_AllowDoubleClick))
	{
		selected = n;

		if (ImGui::IsMouseDoubleClicked(0))
		{
			std::cout << "Load Level: " << buf << std::endl;
			CScene* myUnityScene = CSceneManager::CreateScene(buf);
			CEngine::GetInstance()->AddScene(CStateStack::EState::InGame, myUnityScene);
			CEngine::GetInstance()->SetActiveScene(CStateStack::EState::InGame);

			myIsEnabled = !myIsEnabled;
		}
	}
}
/*	ImGui::TreePop();
}


ImGui::End();
}


*/