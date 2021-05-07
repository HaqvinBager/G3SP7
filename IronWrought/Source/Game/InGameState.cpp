#include "stdafx.h"
#include "InGameState.h"

#include <GameObject.h>
#include <TransformComponent.h>
#include <ModelComponent.h>
#include <DecalComponent.h>
#include <PointLightComponent.h>

#include <Scene.h>
#include <Engine.h>
#include <PostMaster.h>
#include <MainSingleton.h>
#include <CollisionManager.h>
#include <PointLight.h>

#include <JsonReader.h>
#include <SceneManager.h>

#include "EnemyAnimationController.h"

#ifndef NDEBUG
	#include <VFXSystemComponent.h>
	#include <VFXMeshFactory.h>
	#include <ParticleEmitterFactory.h>
	
	#include <TextFactory.h>
	#include <TextInstance.h>
	#include "EnemyComponent.h"
	
	void TEMP_VFX(CScene* aScene);
#endif

CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState),
	myExitLevel(false)
	, myEnemyAnimationController(nullptr)
{
}

CInGameState::~CInGameState() 
{
	delete myEnemyAnimationController;
}


void CInGameState::Awake()
{
	CJsonReader::Get()->InitFromGenerated();
	myEnemyAnimationController = new CEnemyAnimationController();
	CScene* scene = CSceneManager::CreateEmpty();
#ifndef NDEBUG
	TEMP_VFX(scene);
#endif
	CEngine::GetInstance()->AddScene(myState, scene);
	CMainSingleton::PostMaster().Subscribe("Level_1-1", this);
	CMainSingleton::PostMaster().Subscribe("Level_1-2", this);
	CMainSingleton::PostMaster().Subscribe("Level_2-1", this);
	CMainSingleton::PostMaster().Subscribe("Level_2-2", this);

}


void CInGameState::Start()
{
	myEnemyAnimationController->Activate();
	CEngine::GetInstance()->SetActiveScene(myState);
	IRONWROUGHT->GetActiveScene().CanvasIsHUD();
	IRONWROUGHT->HideCursor();
	myExitLevel = false;

	CMainSingleton::PostMaster().Subscribe(PostMaster::MSG_DISABLE_GLOVE, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::MSG_ENABLE_GLOVE, this);
}

void CInGameState::Stop()
{
	IRONWROUGHT->RemoveScene(myState);
	CMainSingleton::CollisionManager().ClearColliders();
	myEnemyAnimationController->Deactivate();

	CMainSingleton::PostMaster().Unsubscribe(PostMaster::MSG_DISABLE_GLOVE, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::MSG_ENABLE_GLOVE, this);
}

void CInGameState::Update()
{
	if (Input::GetInstance()->IsKeyPressed(VK_ESCAPE))
	{
		myStateStack.PushState(CStateStack::EState::PauseMenu);
	}

	DEBUGFunctionality();

	if (myExitLevel)
	{
		myExitLevel = false;
		myStateStack.PopTopAndPush(CStateStack::EState::LoadLevel);
	}
}

void CInGameState::ReceiveEvent(const EInputEvent aEvent)
{
	if (this == myStateStack.GetTop())
	{
		switch (aEvent)
		{
			case IInputObserver::EInputEvent::PauseGame:
				//myStateStack.PushState(CStateStack::EState::PauseMenu);
				break;
			default:
				break;
		}
	}
}

void CInGameState::Receive(const SStringMessage& aMessage)
{
	if (PostMaster::LevelCheck(aMessage.myMessageType))
	{
		myExitLevel = true;
	}

	if (PostMaster::DisableGravityGlove(aMessage.myMessageType))
	{
		IRONWROUGHT->GetActiveScene().CanvasToggle(false);
	}
	if (PostMaster::EnableGravityGlove(aMessage.myMessageType))
	{
		IRONWROUGHT->GetActiveScene().CanvasToggle(true);
	}
	//const char* test = "Level_1-1";
	//if (strcmp(aMessage.myMessageType, test) == 0)
	//{
	//	/*Start();*/
	//	myExitLevel = true;
	//}
}

void CInGameState::Receive(const SMessage& /*aMessage*/)
{
	//switch (aMessage.myMessageType)
	//{
	//	default:break;
	//}
}

void CInGameState::DEBUGFunctionality()
{
#ifdef _DEBUG
	if (Input::GetInstance()->IsKeyPressed('X'))
	{
		SStringMessage msg = {};
		msg.data = nullptr;
		msg.myMessageType = PostMaster::MSG_DISABLE_GLOVE;
		CMainSingleton::PostMaster().Send(msg);
	}
	if (Input::GetInstance()->IsKeyPressed('Z'))
	{
		SStringMessage msg = {};
		msg.data = nullptr;
		msg.myMessageType = PostMaster::MSG_ENABLE_GLOVE;
		CMainSingleton::PostMaster().Send(msg);
	}
#endif
}

#ifndef NDEBUG
	void TEMP_VFX(CScene* aScene)
	{
		static int id = 500;
		CGameObject* abilityObject = new CGameObject(id++);
		abilityObject->AddComponent<CVFXSystemComponent>(*abilityObject, ASSETPATH("Assets/Graphics/VFX/JSON/VFXSystem_ToLoad.json"));
	
		aScene->AddInstance(abilityObject);
		aScene->SetVFXTester(abilityObject);
	}
#endif
