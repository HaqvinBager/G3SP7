#include "stdafx.h"
#include "PauseMenuState.h"

#include "SceneManager.h"
#include "GameObject.h"

#include "PostMaster.h"

CPauseMenuState::CPauseMenuState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState) 
{}

CPauseMenuState::~CPauseMenuState()
{
	std::cout << __FUNCTION__ << std::endl;
	CEngine::GetInstance()->RemoveScene(myState);
}

void CPauseMenuState::Awake()
{
	CScene* scene = CSceneManager::CreateMenuScene("PauseMenu", ASSETPATH("Assets/Graphics/UI/JSON/UI_PauseMenu.json"));

	CEngine::GetInstance()->AddScene(myState, scene);
}

void CPauseMenuState::Start()
{
	CEngine::GetInstance()->SetActiveScene(myState);
	IRONWROUGHT->ShowCursor();
	IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_PauseMenu.json"), true);
	CMainSingleton::PostMaster().Subscribe(EMessageType::MainMenu, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::Resume, this);
}

void CPauseMenuState::Stop()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::MainMenu, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Resume, this);
}

void CPauseMenuState::Update()
{
	/*IRONWROUGHT->ShowCursor();*/
	/*IRONWROUGHT->GetActiveScene().UpdateCanvas();*/

#ifdef _DEBUG
	//if (INPUT->IsKeyPressed('R'))
	//{
	//	IRONWROUGHT->GetActiveScene().ReInitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_PauseMenu.json"));
	//}
#endif

	if (Input::GetInstance()->IsKeyPressed(VK_ESCAPE))
	{
		myStateStack.PopState();
		myStateStack.GetTop()->Start();
	}

}

void CPauseMenuState::Receive(const SStringMessage& /*aMessage*/)
{}

void CPauseMenuState::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::MainMenu:
		{
			if (this->myStateStack.PopUntil(CStateStack::EState::MainMenu, false) == false)
			{
				// This is used when InitDevMode has been called in CGame.
				this->myStateStack.PushState(CStateStack::EState::MainMenu);
			}
		}break;

		case EMessageType::Resume:
		{
			myStateStack.PopState();
			myStateStack.GetTop()->Start();
		}break;

		default:break;
	}
}
