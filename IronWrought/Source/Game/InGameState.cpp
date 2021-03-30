#include "stdafx.h"
#include "InGameState.h"

#include "Scene.h"

#include "GameObject.h"
#include "CameraComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"
#include "SpotLightComponent.h"
#include "BoxLightComponent.h"
#include "TransformComponent.h"
#include "InstancedModelComponent.h"
#include "RigidBodyComponent.h"
#include "ModelComponent.h"
#include "DecalComponent.h"

#include "EnvironmentLight.h"
#include "SpotLight.h"
#include "BoxLight.h"
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
	mySpotlight->GetComponent<CSpotLightComponent>()->GetSpotLight()->SetWideness(abs(sinf(CTimer::Time())));
	//mySpotlight->GetComponent<CSpotLightComponent>()->GetSpotLight()->SetPosition({ 0.0f, abs(sinf(CTimer::Time())), 0.0f });
	//myBoxLight->GetComponent<CBoxLightComponent>()->GetBoxLight()->Rotate({ 2.0f * CTimer::Dt(), 0.0f, 0.0f });

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

void CInGameState::TEMP_Sponza(CScene* aScene)
{
	CGameObject* sponza = new CGameObject(1337);
	sponza->AddComponent<CModelComponent>(*sponza, std::string(ASSETPATH("Assets/Graphics/Sponza/Sponza.fbx")));
	sponza->GetComponent<CTransformComponent>()->Position({ 0.0f,0.0f,0.0f });
	sponza->myTransform->Rotation({ 0.0f,0.0f,0.0f });
	aScene->AddInstance(sponza);

	CGameObject* spotLight = new CGameObject(13414);
	spotLight->myTransform->Position({ 0.0f, 0.9f, -1.5f });
	Vector3 color = { 1.0f, 1.0f, 1.0f };
	Vector3 direction = { 0.0f, -1.0f, 0.0f };
	spotLight->AddComponent<CSpotLightComponent>(*spotLight, color, 16.0f, direction, 1.2f, 1.0f);
	mySpotlight = spotLight;
	aScene->AddInstance(spotLight->GetComponent<CSpotLightComponent>()->GetSpotLight());

	CGameObject* pointLight = new CGameObject(13314);
	Vector3 pointColor = { 1.0f, 1.0f, 1.0f };
	pointLight->myTransform->Position({ 0.0f, 1.0f, 2.0f });
	pointLight->AddComponent<CPointLightComponent>(*pointLight, 1.0f, pointColor, 10.0f);
	auto pl = pointLight->GetComponent<CPointLightComponent>()->GetPointLight();
	pl->SetIntensity(10.0f);
	pl->SetPosition({ 0.0f, 1.0f, 2.0f });
	pl->SetColor(pointColor);
	aScene->AddInstance(pointLight->GetComponent<CPointLightComponent>()->GetPointLight());

	CGameObject* boxLight = new CGameObject(414111);
	//boxLight->myTransform->Position({ -2.0f, 1.0f, 0.0f });
	boxLight->myTransform->Position({ -2.0f, 1.0f, 0.0f });

	Vector3 boxColor = { 0.0f, 1.0f, 1.0f };
	Vector3 boxDirection = { 0.0f, -1.0f, 0.0f };
	auto component = boxLight->AddComponent<CBoxLightComponent>(*boxLight, boxDirection, boxColor, 5.0f, 2.0f);
	component->GetBoxLight()->SetArea({ 0.5f, 0.5f });
	component->GetBoxLight()->SetRotation({ 45.0f, 0.0f, 0.0f });
	//component->GetBoxLight()->SetPosition({ -2.0f, 1.0f, 0.0f });
	myBoxLight = boxLight;
	aScene->AddInstance(boxLight->GetComponent<CBoxLightComponent>()->GetBoxLight());
}
