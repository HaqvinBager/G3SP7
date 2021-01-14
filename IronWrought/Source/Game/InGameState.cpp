#include "stdafx.h"
#include "InGameState.h"

#include "Scene.h"

#include "GameObject.h"
#include "CameraComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"
#include "TransformComponent.h"

#include "ModelComponent.h"
#include "EnvironmentLight.h"
#include "Timer.h"
#include "Engine.h"
#include "PostMaster.h"
#include "WindowHandler.h"
#include "MainSingleton.h"
#include "InputMapper.h"

#include <CollisionManager.h>

#include <iostream>




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

//#include "animationLoader.h" //only for boss test
void CInGameState::Start()
{
	CScene* scene = new CScene();

	CGameObject* camera = new CGameObject(0);
	camera->AddComponent<CCameraComponent>(*camera, 70.0f);
	camera->AddComponent<CCameraControllerComponent>(*camera, 25.0f);
	camera->myTransform->Position({0.0f, 1.0f, 0.0f});
	camera->myTransform->Rotation({0.0f, 0.0f, 0.0f});
	scene->AddInstance(camera);
	scene->SetMainCamera(camera->GetComponent<CCameraComponent>());

	CGameObject* envLight = new CGameObject(1);
	envLight->AddComponent<CEnviromentLightComponent>(*envLight);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetIntensity(1.f);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetDirection({0.0f,0.0f,-1.0f});
	scene->AddInstance(envLight);
	scene->SetEnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());

	CEngine::GetInstance()->AddScene(myState, scene);
	CEngine::GetInstance()->SetActiveScene(myState);

	CGameObject* chest = new CGameObject(1337);
	//chest->AddComponent<CModelComponent>(*chest, "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx");
	chest->AddComponent<CModelComponent>(*chest, std::string(ASSETPATH + "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	chest->GetComponent<CTransformComponent>()->Position({15.0f,0.0f,0.0f});


	CGameObject* dn = new CGameObject(1338);
	dn->AddComponent<CModelComponent>(*dn, "Assets/3D/Exempel_Modeller/DetailNormals/Tufted_Leather/tufted_leather_dn.fbx");
	dn->GetComponent<CTransformComponent>()->Position({0.0f,0.0f,0.0f});
	dn->GetComponent<CTransformComponent>()->Scale(100.0f);

	scene->AddInstance(chest);

	scene->AddInstance(dn);

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