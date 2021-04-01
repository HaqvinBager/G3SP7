#include "stdafx.h"
#include "MainMenuState.h"

#include "SceneManager.h"
#include "GameObject.h"

CMainMenuState::CMainMenuState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
{}

CMainMenuState::~CMainMenuState()
{}

void CMainMenuState::Awake()
{
	CScene* scene = CSceneManager::CreateMenuScene("MainMenu", ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));

	CEngine::GetInstance()->AddScene(myState, scene);
}

void CMainMenuState::Start()
{
	CEngine::GetInstance()->SetActiveScene(myState);
	IRONWROUGHT->ShowCursor();
}

void CMainMenuState::Stop()
{}

void CMainMenuState::Update()
{
	for (auto& gameObject : CEngine::GetInstance()->GetActiveScene().myGameObjects)
	{
		gameObject->Update();
	}

	IRONWROUGHT->GetActiveScene().UpdateCanvas();

	if (INPUT->IsKeyPressed('R'))
	{
		IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_MainMenu.json"));
	}
}

void CMainMenuState::Receive(const SStringMessage& /*aMessage*/)
{}

void CMainMenuState::Receive(const SMessage& /*aMessage*/)
{}
