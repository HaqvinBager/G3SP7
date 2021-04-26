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
	//IRONWROUGHT->ShowCursor(false);
	IRONWROUGHT->GetActiveScene().CanvasToggle(true, true);
	IRONWROUGHT->GetActiveScene().DisableWidgetsOnCanvas();
	CMainSingleton::PostMaster().Subscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Quit, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::SetResolution1280x720, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::SetResolution1600x900, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::SetResolution1920x1080, this);
}

void CMainMenuState::Stop()
{
	IRONWROUGHT->GetActiveScene().CanvasToggle(false, false);
	IRONWROUGHT->GetActiveScene().DisableWidgetsOnCanvas();
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::StartGame, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Quit, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::SetResolution1280x720, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::SetResolution1600x900, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::SetResolution1920x1080, this);
}

void CMainMenuState::Update()
{
#ifndef NDEBUG
	if (INPUT->IsKeyPressed('R'))
	{
		IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));
	}
#endif

	if(myTimeToQuit)
		myStateStack.PopState();
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
			IRONWROUGHT->HideCursor();
			CSceneFactory::Get()->LoadSceneAsync(scene, CStateStack::EState::InGame, [this](std::string aJson) { CMainMenuState::OnComplete(aJson); });
		}break;

		case EMessageType::Quit:
		{
			myTimeToQuit = true;
		}break;

		case EMessageType::SetResolution1280x720:
		{
			CEngine::GetInstance()->SetResolution({ 1280.0f, 720.0f });
			IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));
			myStateStack.PopTopAndPush(CStateStack::EState::MainMenu);
		} break;
		case EMessageType::SetResolution1600x900:
		{
			CEngine::GetInstance()->SetResolution({ 1600.0f, 900.0f });
			IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));
			myStateStack.PopTopAndPush(CStateStack::EState::MainMenu);
		} break;
		case EMessageType::SetResolution1920x1080:
		{
			CEngine::GetInstance()->SetResolution({ 1920.0f, 1080.0f });
			IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));
			myStateStack.PopTopAndPush(CStateStack::EState::MainMenu);
		} break;
		case EMessageType::SetResolution2560x1440:
		{
			CEngine::GetInstance()->SetResolution({ 2560.0f, 1440.0f });
			IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));
			myStateStack.PopTopAndPush(CStateStack::EState::MainMenu);
		} break;

		default: break;
	}
}

void CMainMenuState::OnComplete(std::string aSceneThatHasBeenSuccessfullyLoaded)
{
#ifdef _DEBUG
	std::cout << __FUNCTION__ << "Scene Load Complete!" << aSceneThatHasBeenSuccessfullyLoaded << std::endl;
#endif
	this->myStateStack.PushState(CStateStack::EState::InGame);
	CEngine::GetInstance()->LoadGraph(aSceneThatHasBeenSuccessfullyLoaded);
}
