#include "stdafx.h"
#include "LoadLevelState.h"

#include "FullscreenRenderer.h"
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

	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_LEVEL11, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_LEVEL12, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_LEVEL21, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_LEVEL22, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::StartGame, this);
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
		CheckAndUpdatePostProcess(myLevelToLoad.c_str());
	}
}

void CLoadLevelState::Receive(const SMessage& aMessage)
{
	if (aMessage.myMessageType == EMessageType::StartGame)
	{
		std::string lvl = *static_cast<std::string*>(aMessage.data);
		CheckAndUpdatePostProcess(lvl.c_str());
	}
}

void CLoadLevelState::OnComplete(std::string aSceneThatHasBeenSuccessfullyLoaded)
{
#ifdef _DEBUG
	std::cout << __FUNCTION__ << ": Scene Load Complete! " << aSceneThatHasBeenSuccessfullyLoaded << std::endl;
#endif
	this->myStateStack.PopTopAndPush(CStateStack::EState::InGame);
	CEngine::GetInstance()->LoadGraph(aSceneThatHasBeenSuccessfullyLoaded);
}

void CLoadLevelState::CheckAndUpdatePostProcess(const char* aLevel)
{
	CFullscreenRenderer::SPostProcessingBufferData bufferData = IRONWROUGHT->GetPostProcessingBufferData();
	if (PostMaster::CompareStringMessage(aLevel, PostMaster::SMSG_LEVEL11)
	||  PostMaster::CompareStringMessage(aLevel, PostMaster::SMSG_LEVEL12))
	{
		// Inside
		bufferData.myWhitePointColor = { 255.0f/255.0f, 170.0f/255.0f, 0.5f, 1.0f };
		bufferData.myWhitePointIntensity = 10.0f;
		bufferData.myExposure = 1.0f;
		bufferData.myIsReinhard = false;
		bufferData.myIsUncharted = true;
		bufferData.myIsACES = false;
	}
	else if (PostMaster::CompareStringMessage(aLevel, PostMaster::SMSG_LEVEL21)
		 ||  PostMaster::CompareStringMessage(aLevel, PostMaster::SMSG_LEVEL22))
	{
		// Outside
		bufferData.myWhitePointColor = { 128.0f/255.0f, 170.0f/255.0f, 1.0f, 1.0f };
		bufferData.myWhitePointIntensity = 0.1f;
		bufferData.myExposure = 0.1f;
		bufferData.myIsReinhard = false;
		bufferData.myIsUncharted = true;
		bufferData.myIsACES = false;
	}
	IRONWROUGHT->SetPostProcessingBufferData(bufferData);
}
