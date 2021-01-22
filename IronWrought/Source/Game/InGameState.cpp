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

	//rapidjson::Document document = CJsonReader::LoadDocument(ASSETPATH + "Assets/TestJson.json");
	//auto jsonarray = document["instancedGameobjects"].GetArray();

	//for (auto& jsongameobject : jsonarray) {
	//
	//	CGameObject* instancedGameObject = new CGameObject(0);
	//	std::string model_path;
	//	//float instanceID;
	//	Vector3 position;
	//	Vector3 rotation;
	//	Vector3 scale;
	//
	//	auto jsonmodelpath = jsongameobject["model"].GetObjectW();
	//	model_path = jsonmodelpath["fbxPath"].GetString();
	//	auto jsonTransforms = jsongameobject["transforms"].GetArray();
	//	std::vector<DirectX::SimpleMath::Matrix> instancedTransforms;
	//	for (auto& jsonTransform : jsonTransforms) {
	//		//auto jsoninstanceID = jsontransform["instanceID"].GetObjectW();
	//		auto jsonposition = jsonTransform["position"].GetObjectW();
	//		auto jsonrotation = jsonTransform["rotation"].GetObjectW();
	//		auto jsonscale = jsonTransform["scale"].GetObjectW();

	//		//instanceID = jsoninstanceID[""].GetFloat();

	//		position.x = jsonposition["x"].GetFloat();			
	//		position.y = jsonposition["y"].GetFloat();
	//		position.z = jsonposition["z"].GetFloat();

	//		rotation.x = jsonrotation["x"].GetFloat();
	//		rotation.y = jsonrotation["y"].GetFloat();
	//		rotation.z = jsonrotation["z"].GetFloat();

	//		scale.x = jsonscale["x"].GetFloat();
	//		scale.y = jsonscale["y"].GetFloat();
	//		scale.z = jsonscale["z"].GetFloat();

	//		CGameObject temp(0);
	//		CTransformComponent transform(temp);
	//		transform.Scale(scale.x);
	//		transform.Position(position);
	//		transform.Rotation(rotation);
	//		instancedTransforms.emplace_back(transform.GetMatrix());
	//		
	//	}
	//	instancedGameObject->AddComponent<CInstancedModelComponent>(*instancedGameObject, std::string(ASSETPATH + model_path), instancedTransforms);
	//	scene->AddInstance(instancedGameObject);
	//}

	//goal
	// flytta inladdningen/inläsningen av objekt till sin egna klass för att tömma ingamestate på onödig kod

	







	/*scene = CSceneManager::CreateScene("TestJson.json");
	CEngine::GetInstance()->AddScene(myState, scene);*/



	//CGameObject* chest = new CGameObject(1337);
	//chest->AddComponent<CModelComponent>(*chest, "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx");
	//chest->AddComponent<CModelComponent>(*chest, std::string(ASSETPATH + "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	//chest->GetComponent<CTransformComponent>()->Position({4.0f,0.0f,0.0f});

	//CGameObject* chest2 = new CGameObject(1338);
	//chest2->AddComponent<CModelComponent>(*chest2, "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx");
	//chest2->AddComponent<CModelComponent>(*chest2, std::string(ASSETPATH + "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	//chest2->GetComponent<CTransformComponent>()->Position({5.0f,-2.0f,0.0f});

	//CGameObject* chest3 = new CGameObject(1339);
	//chest3->AddComponent<CModelComponent>(*chest3, "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx");
	//chest3->AddComponent<CModelComponent>(*chest3, std::string(ASSETPATH + "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	//chest3->GetComponent<CTransformComponent>()->Position({6.0f,2.0f,0.0f});

	//scene->AddInstance(chest);
	//scene->AddInstance(chest2);
	//scene->AddInstance(chest3);

	//std::vector<CGameObject*> pointLights;
	//for (int i = 0; i < 5; ++i)
	//{
	//	CGameObject* pl = new CGameObject(9999 + i);
	//	/*CPointLightComponent* plL = */pl->AddComponent<CPointLightComponent>(*pl, 10.f, SM::Vector3{1,1,1}, 10.f);
	//	pointLights.emplace_back(pl);
	//	pl->myTransform->Position({ 0xDEAD, 0xDEAD, 0xDEAD });
	//	
	//}
	//pointLights[0]->myTransform->Position({ 6.0f, 0.0f, 0.0f });
	//pointLights[0]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 0.0f,1.0f,0.0f });
	//pointLights[0]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 4.0f,0.0f,-3.0f });

	//pointLights[1]->myTransform->Position({ 4.0f, 1.0f, 0.0f });
	//pointLights[1]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 1.0f,0.0f,0.0f });
	//pointLights[1]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 5.0f,2.0f,-1.0f });
	//
	//pointLights[2]->myTransform->Position({ 7.0f, 2.0f, 0.0f });
	//pointLights[2]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 0.0f,0.0f,1.0f });
	//pointLights[2]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 4.0f,-1.0f,-2.0f });
	//
	//pointLights[3]->myTransform->Position({ 6.0f, 0.0f, 0.0f });
	//pointLights[3]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 0.0f,0.5f,1.0f });
	//pointLights[3]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 4.0f,0.0f,-2.0f });

	//pointLights[4]->myTransform->Position({ 10.0f, 2.0f, 0.0f });
	//pointLights[4]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 0.5f,0.0f,1.0f });
	//pointLights[4]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 5.0f,-2.0f,-2.0f });

	//scene->AddInstance(pointLights[0]->GetComponent<CPointLightComponent>()->GetPointLight());
	//scene->AddInstance(pointLights[1]->GetComponent<CPointLightComponent>()->GetPointLight());
	//scene->AddInstance(pointLights[2]->GetComponent<CPointLightComponent>()->GetPointLight());
	//scene->AddInstance(pointLights[3]->GetComponent<CPointLightComponent>()->GetPointLight());
	//scene->AddInstance(pointLights[4]->GetComponent<CPointLightComponent>()->GetPointLight());

	//CGameObject* dn = new CGameObject(1338);
	//dn->AddComponent<CModelComponent>(*dn, "Assets/3D/Exempel_Modeller/DetailNormals/Tufted_Leather/tufted_leather_dn.fbx");
	//dn->GetComponent<CTransformComponent>()->Position({7.0f,100.0f,0.0f});
	//dn->GetComponent<CTransformComponent>()->Scale(100.0f);

	//scene->AddInstance(dn);

	//steg 1. kalla p� read json funktion
	//steg 2. skapa en gameobject struct
	//steg 3.

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
