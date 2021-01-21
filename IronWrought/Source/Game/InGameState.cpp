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
	CScene* scene = new CScene();
	scene->AddPXScene(CMainSingleton::PhysXWrapper().CreatePXScene());

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

	//rapidjson::Document document = CJsonReader::LoadDocument(ASSETPATH + "Assets/TestJson.json");
	//auto jsonarray = document["gameobjects"].GetArray();

	//for (auto& jsongameobject : jsonarray) {
	//
	//	CGameObject* gameobject = new CGameObject(0);
	//	std::string model_path;
	//	//float instanceID;
	//	Vector3 position;
	//	Vector3 rotation;
	//	Vector3 scale;
	//
	//	auto jsonmodelpath = jsongameobject["model"].GetObjectW();
	//
	//	model_path = jsonmodelpath["fbxPath"].GetString();
	//
	//	auto jsontransform = jsongameobject["transform"].GetObjectW();
	//	//auto jsoninstanceID = jsontransform["instanceID"].GetObjectW();
	//	auto jsonposition = jsontransform["position"].GetObjectW();
	//	auto jsonrotation = jsontransform["rotation"].GetObjectW();
	//	auto jsonscale = jsontransform["scale"].GetObjectW();
	//
	//	//instanceID = jsoninstanceID[""].GetFloat();
	//
	//	position.x = jsonposition["x"].GetFloat();
	//	position.y = jsonposition["y"].GetFloat();
	//	position.z = jsonposition["z"].GetFloat();
	//	
	//	rotation.x = jsonrotation["x"].GetFloat();
	//	rotation.y = jsonrotation["y"].GetFloat();
	//	rotation.z = jsonrotation["z"].GetFloat();
	//
	//	scale.x = jsonscale["x"].GetFloat();
	//	scale.y = jsonscale["y"].GetFloat();
	//	scale.z = jsonscale["z"].GetFloat();
	//
	//	gameobject->AddComponent<CModelComponent>(*gameobject, std::string(ASSETPATH + model_path));
	//	gameobject->GetComponent<CTransformComponent>()->Position(position);
	//	gameobject->GetComponent<CTransformComponent>()->Rotation(rotation);
	//	gameobject->GetComponent<CTransformComponent>()->Scale(scale.x);
	//
	//
	//	scene->AddInstance(gameobject);
	//}


	CGameObject* chest = new CGameObject(1337);
	chest->AddComponent<CModelComponent>(*chest, std::string(ASSETPATH + "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	chest->GetComponent<CTransformComponent>()->Position({4.0f,0.0f,0.0f});

	CGameObject* chest2 = new CGameObject(1338);
	chest2->AddComponent<CModelComponent>(*chest2, std::string(ASSETPATH + "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	chest2->GetComponent<CTransformComponent>()->Position({5.0f,-2.0f,0.0f});

	CGameObject* chest3 = new CGameObject(1339);
	chest3->AddComponent<CModelComponent>(*chest3, std::string(ASSETPATH + "Assets/3D/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	chest3->GetComponent<CTransformComponent>()->Position({6.0f,2.0f,0.0f});

	scene->AddInstance(chest);
	scene->AddInstance(chest2);
	scene->AddInstance(chest3);

	constexpr int numPointLights = 0;
	std::vector<CGameObject*> pointLights;
	float x = -2.0f;
	float y = -10.0f;
	for (int i = 0; i < numPointLights; ++i)
	{
		if ((i + 1) % 10 == 0)
		{
			x = -2.0f;
			y += 1.0f;
		}
		x -= 1.0f;

		CGameObject* pl = new CGameObject(1789 + i);
		pl->AddComponent<CPointLightComponent>(*pl, 10.f, SM::Vector3{1,1,1}, 10.f);
		pl->myTransform->Position({ x, y, -3.0f });

		int thirdRange = numPointLights / 3;
		float r = (i < thirdRange ? 1.0f : 0.0f);
		float g = (i > thirdRange && i < thirdRange * 2 ? 1.0f : 0.0f);
		float b = (i > thirdRange * 2 && i < thirdRange * 3 ? 1.0f : 0.0f);
		pl->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ r,g,b });
		pl->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ x, y, -3.0f });

		scene->AddInstance(pl->GetComponent<CPointLightComponent>()->GetPointLight());
		scene->AddInstance(pl);
	}
	for (int i = 0; i < 5; ++i)
	{
		CGameObject* pl = new CGameObject(9999 + i);
		pl->AddComponent<CPointLightComponent>(*pl, 10.f, SM::Vector3{1,1,1}, 10.f);
		pointLights.emplace_back(pl);
		pl->myTransform->Position({ 0xDEAD, 0xDEAD, 0xDEAD });
		
	}
	pointLights[0]->myTransform->Position({ 6.0f, 0.0f, 0.0f });
	pointLights[0]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 0.0f,1.0f,0.0f });
	pointLights[0]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 4.0f,0.0f,-3.0f });

	pointLights[1]->myTransform->Position({ 4.0f, 1.0f, 0.0f });
	pointLights[1]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 1.0f,0.0f,0.0f });
	pointLights[1]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 5.0f,2.0f,-1.0f });
	
	pointLights[2]->myTransform->Position({ 7.0f, 2.0f, 0.0f });
	pointLights[2]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 0.0f,0.0f,1.0f });
	pointLights[2]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 4.0f,-1.0f,-2.0f });
	
	pointLights[3]->myTransform->Position({ 6.0f, 0.0f, 0.0f });
	pointLights[3]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 0.0f,0.5f,1.0f });
	pointLights[3]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 4.0f,0.0f,-2.0f });

	pointLights[4]->myTransform->Position({ 10.0f, 2.0f, 0.0f });
	pointLights[4]->GetComponent<CPointLightComponent>()->GetPointLight()->SetColor({ 0.5f,0.0f,1.0f });
	pointLights[4]->GetComponent<CPointLightComponent>()->GetPointLight()->SetPosition({ 5.0f,-2.0f,-2.0f });

	scene->AddInstance(pointLights[0]->GetComponent<CPointLightComponent>()->GetPointLight());
	scene->AddInstance(pointLights[1]->GetComponent<CPointLightComponent>()->GetPointLight());
	scene->AddInstance(pointLights[2]->GetComponent<CPointLightComponent>()->GetPointLight());
	scene->AddInstance(pointLights[3]->GetComponent<CPointLightComponent>()->GetPointLight());
	scene->AddInstance(pointLights[4]->GetComponent<CPointLightComponent>()->GetPointLight());

	CGameObject* dn = new CGameObject(1338);
	dn->AddComponent<CModelComponent>(*dn, "Assets/3D/Exempel_Modeller/DetailNormals/Tufted_Leather/tufted_leather_dn.fbx");
	dn->GetComponent<CTransformComponent>()->Position({7.0f,0.0f,0.0f});
	dn->GetComponent<CTransformComponent>()->Scale(100.0f);

	CGameObject* dn4 = new CGameObject(1339);
	dn4->AddComponent<CModelComponent>(*dn4, "Assets/3D/Exempel_Modeller/DetailNormals/4DN/4DNs_dn.fbx");
	dn4->GetComponent<CTransformComponent>()->Position({8.0f,0.0f,0.0f});
	dn4->GetComponent<CTransformComponent>()->Scale(100.0f);

	scene->AddInstance(dn);
	scene->AddInstance(dn4);

	constexpr int instancedCount = 300;
	std::vector<SM::Matrix> transforms(instancedCount);
	x = -2.0f;
	y = -10.0f;
	for (int i = 0; i < instancedCount; ++i)
	{
		if ((i + 1) % 10 == 0)
		{
			x = -2.0f;
			y += 1.0f;
		}
		x -= 1.0f;

		SM::Matrix transform;
		transform.Translation({ x, y, 0.0f });
		transforms[i] = transform;


	}
	CGameObject* instancedGameObject = new CGameObject(999);
	instancedGameObject->AddComponent<CInstancedModelComponent>(*instancedGameObject
													   , "Assets/3D/Exempel_Modeller/DetailNormals/Tufted_Leather/tufted_leather_dn.fbx"
													   , instancedCount
													   , transforms
													   , false);
	scene->AddInstance(instancedGameObject);

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
