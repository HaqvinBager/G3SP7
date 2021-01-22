#include "stdafx.h"
#include "InGameState.h"

#include "Scene.h"

#include "GameObject.h"
#include "CameraComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"
#include "TransformComponent.h"
#include "InstancedModelComponent.h"
#include "RigidBodyComponent.h"
#include "ModelComponent.h"

#include "EnvironmentLight.h"
#include "Timer.h"
#include "Engine.h"
#include "PostMaster.h"
#include "WindowHandler.h"
#include "MainSingleton.h"
#include "InputMapper.h"

#include <CollisionManager.h>

#include <JsonReader.h>
#include <iostream>
#include "SceneManager.h"




CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState),
	myExitLevel(false)
{
}

CInGameState::~CInGameState()
{
 }

void CInGameState::Awake()
{

}

#include "PointLight.h"
#include "PointLightComponent.h"

void CInGameState::Start()
{

// gör en till scene med chests som heter lvl 2
// gör så att man kan byta utkommenterad kod för att byta mellan lvl 1 och lvl 2 inladdning


	CScene* myUnityScene = CSceneManager::CreateScene("TestJson.json");
	CEngine::GetInstance()->AddScene(myState, myUnityScene);
	CEngine::GetInstance()->SetActiveScene(myState);

	myExitLevel = false;

	std::vector<CGameObject*>& gameObjects = CEngine::GetInstance()->GetActiveScene().myGameObjects;
	size_t currentSize = gameObjects.size();
	for (size_t i = 0; i < currentSize; ++i)
	{
		if (gameObjects[i])
		{
			gameObjects[i]->Awake();
		}
	}

	////Late awake
	size_t newSize = gameObjects.size();
	for (size_t j = currentSize; j < newSize; ++j)
	{
		if (gameObjects[j])
		{
			gameObjects[j]->Awake();
		}
	}

	for (auto& gameObject : CEngine::GetInstance()->GetActiveScene().myGameObjects)
	{
		gameObject->Start();
	}

	CEngine::GetInstance()->GetActiveScene().GetMainCamera()->Fade(true);
}

void CInGameState::Stop()
{
	CMainSingleton::CollisionManager().ClearColliders();
}

void CInGameState::Update()
{
	CMainSingleton::PhysXWrapper().Simulate();
	for (auto& gameObject : CEngine::GetInstance()->GetActiveScene().myGameObjects)
	{
		gameObject->Update();
	}
}

void CInGameState::ReceiveEvent(const EInputEvent aEvent)
{
	if (this == myStateStack.GetTop()) {
		switch (aEvent) {
		case IInputObserver::EInputEvent::PauseGame:
			break;
		default:
			break;
		}
	}
}

void CInGameState::Receive(const SStringMessage& /*aMessage*/)
{
	myExitLevel = true;
}

void CInGameState::Receive(const SMessage& /*aMessage*/)
{
	//switch (aMessage.myMessageType)
	//{
	//	default:break;
	//}
}
