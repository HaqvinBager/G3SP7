#include "stdafx.h"
#include "LoadLevelState.h"
//#include "EngineDefines.h"

#include "SceneManager.h"
#include "GameObject.h"

CLoadLevelState::CLoadLevelState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
{}

CLoadLevelState::~CLoadLevelState()
{}

void CLoadLevelState::Awake()
{
	CScene* scene = CSceneManager::CreateMenuScene("MainMenu", ASSETPATH("Assets/Graphics/UI/JSON/UI_LoadingScreen.json"));
	CEngine::GetInstance()->AddScene(myState, scene);

	CMainSingleton::PostMaster().Subscribe("Level_1-1", this);
	CMainSingleton::PostMaster().Subscribe("Level_1-2", this);
	CMainSingleton::PostMaster().Subscribe("Level_2-1", this);
	CMainSingleton::PostMaster().Subscribe("Level_2-2", this);
}

void CLoadLevelState::Start()
{
	CEngine::GetInstance()->SetActiveScene(myState);
	IRONWROUGHT->HideCursor();
	CSceneFactory::Get()->LoadSceneAsync(myLevelToLoad, CStateStack::EState::InGame, [this](std::string aJson) { CLoadLevelState::OnComplete(aJson); });
}

void CLoadLevelState::Stop()
{}

void CLoadLevelState::Update()
{}

void CLoadLevelState::Receive(const SStringMessage& aMessage)
{
	if (PostMaster::LevelCheck(aMessage.myMessageType))
	{
		myLevelToLoad = aMessage.myMessageType;
	}
}

void CLoadLevelState::Receive(const SMessage& /*aMessage*/)
{}

void CLoadLevelState::OnComplete(std::string aSceneThatHasBeenSuccessfullyLoaded)
{
#ifdef _DEBUG
	std::cout << __FUNCTION__ << ": Scene Load Complete! " << aSceneThatHasBeenSuccessfullyLoaded << std::endl;
#endif
	this->myStateStack.PopTopAndPush(CStateStack::EState::InGame);
	CEngine::GetInstance()->LoadGraph(aSceneThatHasBeenSuccessfullyLoaded);
}
