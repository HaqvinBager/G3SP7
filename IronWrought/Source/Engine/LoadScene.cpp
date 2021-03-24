#include "stdafx.h"
#include "LoadScene.h"

#include "JsonReader.h"
#include "Scene.h"
#include "Engine.h"
#include "SceneManager.h"
#include "CameraComponent.h"
#include "FolderUtility.h"
#include "MainSingleton.h"
#include "PostMaster.h"

IronWroughtImGui::CLoadScene::CLoadScene(const char* aMenuName, const bool aIsMenuChild)
	: CWindow(aMenuName, aIsMenuChild)
	, myState(EState::DropDownMenu)
	, mySceneIndex(-1)
{
	OnEnable();

}

IronWroughtImGui::CLoadScene::~CLoadScene()
{
}

void IronWroughtImGui::CLoadScene::OnEnable()
{
	std::vector<std::string> generatedJsonFiles = CFolderUtility::GetFileNamesInFolder(ASSETPATH ("Assets/Generated"), ".json"/*, "Level"*/);
	for (auto& file : generatedJsonFiles) {
		//auto endIndex = file.find_last_of('_');
		//std::string sceneName = file.substr(0, endIndex);
		if (std::find(myScenes.begin(), myScenes.end(), file.c_str()) == myScenes.end()) {
			myScenes.emplace_back(file.c_str());
		}
	}
	myScenes.push_back("Empty");
	myState = EState::DropDownMenu;



}


bool IronWroughtImGui::CLoadScene::OnMainMenuGUI()
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
				{
					newScene = CSceneManager::CreateEmpty();
					CEngine::GetInstance()->AddScene(CStateStack::EState::InGame, newScene);
					CEngine::GetInstance()->SetActiveScene(CStateStack::EState::InGame);
				}
				else
				{
					CSceneFactory::Get()->LoadSceneAsync(scene, [this]() { CLoadScene::OnComplete(); });
					//newScene = CSceneManager::CreateScene(scene);
				}
				
				//CEngine::GetInstance()->AddScene(CStateStack::EState::InGame, newScene);
				//CEngine::GetInstance()->SetActiveScene(CStateStack::EState::InGame);



			}
			index++;
		}
		ImGui::EndCombo();
	}


	return true;
}

void IronWroughtImGui::CLoadScene::OnInspectorGUI()
{
}

void IronWroughtImGui::CLoadScene::OnDisable()
{
}

void IronWroughtImGui::CLoadScene::OnComplete()
{
	std::cout << "Complete omfg!" << std::endl;
	//CCameraComponent* newCamera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
	//newCamera->GameObject().myTransform->Position(camPos);
	CMainSingleton::PostMaster().Send({ "LoadScene", nullptr });
	myScenes.clear();
	OnEnable();
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