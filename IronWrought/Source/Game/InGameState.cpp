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
#include "DecalComponent.h"

#include "EnvironmentLight.h"
#include "Timer.h"
#include "Engine.h"
#include "PostMaster.h"
#include "WindowHandler.h"
#include "MainSingleton.h"
#include "InputMapper.h"

#include <CollisionManager.h>
#include "ImguiManager.h"

#include <JsonReader.h>
#include <iostream>
#include "SceneManager.h"
#include "FolderUtility.h"

#include "animationLoader.h"

#include "VFXSystemComponent.h"
#include "VFXMeshFactory.h"
#include "ParticleEmitterFactory.h"

void TEMP_Sponza(CScene* aScene);

CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState),
	myExitLevel(false)
{
}

CInGameState::~CInGameState(){}

void CInGameState::Awake(){}

#include "PointLight.h"
#include "PointLightComponent.h"
#include "PlayerControllerComponent.h"
void CInGameState::Start()
{
	CJsonReader::Get()->Init();
	CScene* scene = CSceneManager::CreateEmpty();
	scene->AddPXScene(CEngine::GetInstance()->GetPhysx().CreatePXScene());
	
	TEMP_Sponza(scene);

	CEngine::GetInstance()->AddScene(myState, scene);
	CEngine::GetInstance()->SetActiveScene(myState);

	myExitLevel = false;
}

void CInGameState::Stop()
{
	CMainSingleton::CollisionManager().ClearColliders();
}

void CInGameState::Update()
{
	CEngine::GetInstance()->GetPhysx().Simulate();
	for (auto& gameObject : CEngine::GetInstance()->GetActiveScene().myGameObjects)
	{
		gameObject->Update();
	}

	for (auto& gameObject : CEngine::GetInstance()->GetActiveScene().myGameObjects)
	{
		gameObject->LateUpdate();
	}

	if (Input::GetInstance()->IsKeyPressed(VK_ESCAPE))
	{
		myStateStack.PopTopAndPush(CStateStack::EState::InGame);
	}
}

void CInGameState::ReceiveEvent(const EInputEvent aEvent)
{
	if (this == myStateStack.GetTop())
	{
		switch (aEvent)
		{
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

void TEMP_Sponza(CScene* aScene)
{
	CGameObject* sponza = new CGameObject(1337);
	sponza->AddComponent<CModelComponent>(*sponza, std::string(ASSETPATH("Assets/Graphics/Sponza/Sponza.fbx")));
	sponza->GetComponent<CTransformComponent>()->Position({ 0.0f,0.0f,0.0f });
	sponza->myTransform->Rotation({ 0.0f,0.0f,0.0f });
	aScene->AddInstance(sponza);
}
