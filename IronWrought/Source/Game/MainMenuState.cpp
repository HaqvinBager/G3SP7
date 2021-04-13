#include "stdafx.h"
#include "MainMenuState.h"
#include "EngineDefines.h"

#include "SceneManager.h"
#include "GameObject.h"

#include "JsonReader.h"
#include "Scene.h"
#include "Engine.h"
#include "FolderUtility.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CMainMenuState::CMainMenuState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
	, myTimeToQuit(false)
{}

CMainMenuState::~CMainMenuState()
{
	std::cout << __FUNCTION__ << std::endl;
	CEngine::GetInstance()->RemoveScene(myState);
}

void CMainMenuState::Awake()
{
	CScene* scene = CSceneManager::CreateMenuScene("MainMenu", ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));

	CEngine::GetInstance()->AddScene(myState, scene);
}

void CMainMenuState::Start()
{
	CEngine::GetInstance()->SetActiveScene(myState);
	IRONWROUGHT->ShowCursor();
	CMainSingleton::PostMaster().Subscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Quit, this);
}

void CMainMenuState::Stop()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Quit, this);
}

void CMainMenuState::Update()
{
	IRONWROUGHT->ShowCursor();
	for (auto& gameObject : CEngine::GetInstance()->GetActiveScene().myGameObjects)
	{
		gameObject->Update();
	}

	IRONWROUGHT->GetActiveScene().UpdateCanvas();

#ifndef NDEBUG
	if (INPUT->IsKeyPressed('R'))
	{
		IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));
	}
#endif

	if(myTimeToQuit)
		this->myStateStack.PopState();
}

void CMainMenuState::Receive(const SStringMessage& /*aMessage*/)
{
}

void CMainMenuState::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::StartGame:
		{
			std::string scene = *reinterpret_cast<std::string*>(aMessage.data);
#ifdef _DEBUG
			std::cout << __FUNCTION__ << " Loading scene: " << scene << std::endl;
#endif
			CSceneFactory::Get()->LoadScene(scene, CStateStack::EState::InGame, [this](std::string aJson) { CMainMenuState::OnComplete(aJson); });
			this->myStateStack.PushState(CStateStack::EState::InGame);
		}break;

		case EMessageType::Quit:
		{
			myTimeToQuit = true;
		}break;

		default: break;
	}
}

void CMainMenuState::OnComplete(std::string aSceneThatHasBeenSuccessfullyLoaded)
{
#ifdef _DEBUG
	std::cout << __FUNCTION__ << "Scene Load Complete!" << aSceneThatHasBeenSuccessfullyLoaded << std::endl;
#endif

	CEngine::GetInstance()->LoadGraph(aSceneThatHasBeenSuccessfullyLoaded);
	CMainSingleton::PostMaster().Send({ "LoadScene", nullptr });
}
