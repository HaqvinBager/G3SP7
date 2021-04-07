#include "stdafx.h"
#include "MainMenuState.h"

#include "SceneManager.h"
#include "GameObject.h"

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

	if (INPUT->IsKeyPressed('R'))
	{
		IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));
	}

	if(myTimeToQuit)
		this->myStateStack.PopState();
}

void CMainMenuState::Receive(const SStringMessage& /*aMessage*/)
{}

void CMainMenuState::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::StartGame:
		{
			this->myStateStack.PushState(CStateStack::EState::InGame);
		}break;

		case EMessageType::Quit:
		{
			myTimeToQuit = true;
		}break;

		default: break;
	}
}
