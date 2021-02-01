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

#include "AnimationComponent.h"
#include "AnimationController.h"

void TEMP_DeferredRenderingTests(CScene* aScene);
void TEMP_SetUpAnimationTest(CScene* aScene);
void TEMP_AnimObjectControl();

CGameObject* g_TempAnimObject = nullptr;

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
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetColor({1.0f,1.0f,1.0f});
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetIntensity(1.f);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetDirection({1.0f,0.5f,-1.0f});
	scene->AddInstance(envLight);
	scene->SetEnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());

	CEngine::GetInstance()->AddScene(myState, scene);
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


	TEMP_DeferredRenderingTests(scene);
	TEMP_SetUpAnimationTest(scene);

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

	TEMP_AnimObjectControl();
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

void TEMP_DeferredRenderingTests(CScene* scene)
{
	CGameObject* chest = new CGameObject(1337);
	chest->AddComponent<CModelComponent>(*chest, std::string(ASSETPATH + "Assets/Graphics/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	chest->GetComponent<CTransformComponent>()->Position({4.0f,0.0f,0.0f});

	CGameObject* chest2 = new CGameObject(1338);
	chest2->AddComponent<CModelComponent>(*chest2, std::string(ASSETPATH + "Assets/Graphics/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	chest2->GetComponent<CTransformComponent>()->Position({5.0f,-2.0f,0.0f});

	CGameObject* chest3 = new CGameObject(1339);
	chest3->AddComponent<CModelComponent>(*chest3, std::string(ASSETPATH + "Assets/Graphics/Exempel_Modeller/Chest/Particle_Chest.fbx"));
	chest3->GetComponent<CTransformComponent>()->Position({6.0f,2.0f,0.0f});

	//scene->AddInstance(chest);
	//scene->AddInstance(chest2);
	//scene->AddInstance(chest3);

	constexpr int numPointLights = 32;
	std::vector<CGameObject*> pointLights;
	float x = -2.0f;
	float y = 1.0f;
	for (int i = 0; i < numPointLights; ++i)
	{
		if ((i + 1) % 10 == 0)
		{
			x = -2.0f;
			y += 1.0f;
		}
		x -= 1.0f;

		CGameObject* pl = new CGameObject(1789 + i);
		pl->AddComponent<CPointLightComponent>(*pl, 15.f, SM::Vector3{1,1,1}, 10.f);
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
	dn->AddComponent<CModelComponent>(*dn, ASSETPATH + "Assets/Graphics/Exempel_Modeller/DetailNormals/Tufted_Leather/tufted_leather_dn.fbx");
	dn->GetComponent<CTransformComponent>()->Position({7.0f,0.0f,0.0f});
	dn->GetComponent<CTransformComponent>()->Scale(100.0f);

	CGameObject* dn4 = new CGameObject(1339);
	dn4->AddComponent<CModelComponent>(*dn4, ASSETPATH + "Assets/Graphics/Exempel_Modeller/DetailNormals/4DN/4DNs_dn.fbx");
	dn4->GetComponent<CTransformComponent>()->Position({8.0f,0.0f,0.0f});
	dn4->GetComponent<CTransformComponent>()->Scale(100.0f);

	scene->AddInstance(dn);
	scene->AddInstance(dn4);

	constexpr int instancedCount = 300;
	std::vector<SM::Matrix> transforms(instancedCount);
	x = -3.0f;
	y = -10.0f;
	for (int i = 0; i < instancedCount; ++i)
	{
		x -= 3.0f;
		if ((i + 1) % 10 == 0)
		{
			x = -3.0f;
			y += 3.0f;
		}

		SM::Matrix transform;
		transforms[i] = transform;
		transforms[i] *= SM::Matrix::CreateScale(0.01f);
		transforms[i].Translation({ x, y, 0.0f });
	}
	CGameObject* instancedGameObject = new CGameObject(999);
	instancedGameObject->AddComponent<CInstancedModelComponent>(*instancedGameObject
																, std::string(ASSETPATH + "Assets/Graphics/Exempel_Modeller/Wall/Wall.fbx")
																//, "Assets/Graphics/Exempel_Modeller/DetailNormals/4DN/4DNs_dn.fbx"
																, transforms
																, false);
	//scene->AddInstance(instancedGameObject);
}

void TEMP_SetUpAnimationTest(CScene* aScene)
{
	CGameObject* animObj = new CGameObject(123123123);
	//std::string rig = "Assets/Temp/Undead/Undead.fbx";
	//std::vector<std::string> someAnimations;
	//someAnimations.emplace_back("Assets/Temp/Undead/Idle.fbx");
	//someAnimations.emplace_back("Assets/Temp/Undead/Walk.fbx");

	std::string rig = "Assets/Temp/Mixamo/SK.fbx";
	std::vector<std::string> someAnimations;
	someAnimations.emplace_back("Assets/Temp/Mixamo/Defeated_30fps.fbx");	//1
	someAnimations.emplace_back("Assets/Temp/Mixamo/Guitar_30fps.fbx");		//2
	someAnimations.emplace_back("Assets/Temp/Mixamo/Idle_30fps.fbx");		//3
	someAnimations.emplace_back("Assets/Temp/Mixamo/Nervous_30fps.fbx");	//4
	someAnimations.emplace_back("Assets/Temp/Mixamo/Oof_30fps.fbx");		//5
	someAnimations.emplace_back("Assets/Temp/Mixamo/Jump_30fps.fbx");		//6

	animObj->AddComponent<CModelComponent>(*animObj, rig);
	
	CAnimationComponent* animComp = animObj->AddComponent<CAnimationComponent>(*animObj, rig, someAnimations);

	animComp->BlendLerpBetween(4, 5, 0.0f);
	
	g_TempAnimObject = animObj;

	aScene->AddInstance(animObj);
}
#define GetAnimComp g_TempAnimObject->GetComponent<CAnimationComponent>()
void TEMP_AnimObjectControl()
{
	float blend = 1.0f;// >1.0f is scary
	if (Input::GetInstance()->IsKeyPressed('1'))
	{
		//GetAnimComp->SetBlend(1, 5, blend);
		GetAnimComp->BlendToAnimation(1, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('2'))
	{
		//GetAnimComp->SetBlend(2, 5, blend);
		GetAnimComp->BlendToAnimation(2, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('3'))
	{
		//GetAnimComp->SetBlend(3, 5, blend);
		GetAnimComp->BlendToAnimation(3, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('4'))
	{
		//GetAnimComp->SetBlend(4, 5, blend);
		GetAnimComp->BlendToAnimation(4, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('5'))
	{
		//GetAnimComp->SetBlend(5, 5, blend);
		GetAnimComp->BlendToAnimation(5, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('6'))
	{
		//GetAnimComp->SetBlend(6, 5, blend);
		GetAnimComp->BlendToAnimation(6, blend);
	}

	if (Input::GetInstance()->IsKeyPressed(VK_F2))
	{
		GetAnimComp->BlendLerpBetween(4, 5, 0.0f);
	}
	if (Input::GetInstance()->IsKeyPressed(VK_F3))
	{
		GetAnimComp->BlendLerpBetween(1, 3, 0.0f);
	}
	if (Input::GetInstance()->IsKeyPressed(VK_F4))
	{
		GetAnimComp->BlendLerpBetween(2, 6, 0.0f);
	}

	static float blendLerp = 0.0f;
	if (Input::GetInstance()->IsKeyPressed('9'))
	{
		//GetAnimComp->SetBlend(6, 5, blend);
		GetAnimComp->BlendLerp(blendLerp += 0.1f);
		std::cout << __FUNCTION__ << " blendLerp @ " << blendLerp << std::endl;
	}
	if (Input::GetInstance()->IsKeyPressed('0'))
	{
		//GetAnimComp->SetBlend(6, 5, blend);
		GetAnimComp->BlendLerp(blendLerp -= 0.1f);
		std::cout << __FUNCTION__ << " blendLerp @ " << blendLerp << std::endl;
	}
}
