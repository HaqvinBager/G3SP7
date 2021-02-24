#include "stdafx.h"
#include "InGameState.h"

#include "Scene.h"
#include "SceneManager.h"

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
	CScene* scene = CSceneManager::CreateEmpty();

	CEngine::GetInstance()->AddScene(myState, scene);
	CEngine::GetInstance()->SetActiveScene(myState);

	//TEMP_DeferredRenderingTests(scene);
	//TEMP_SetUpAnimationTest(scene);

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

	CEngine::GetInstance()->GetActiveScene().MainCamera()->Fade(true);
}

void CInGameState::Stop()
{
	CMainSingleton::CollisionManager().ClearColliders();
}

void CInGameState::Update()
{
	//TEMP_AnimObjectControl();

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
	constexpr int numPointLights = 6;
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
	dn->AddComponent<CModelComponent>(*dn, ASSETPATH("Assets/Graphics/Exempel_Modeller/DetailNormals/Tufted_Leather/tufted_leather_dn.fbx"));
	dn->GetComponent<CTransformComponent>()->Position({7.0f,0.0f,0.0f});
	dn->GetComponent<CTransformComponent>()->Scale({100.0f,100.0f,100.0f});

	CGameObject* dn4 = new CGameObject(1339);
	dn4->AddComponent<CModelComponent>(*dn4, ASSETPATH("Assets/Graphics/Exempel_Modeller/DetailNormals/4DN/4DNs_dn.fbx"));
	dn4->GetComponent<CTransformComponent>()->Position({8.0f,0.0f,0.0f});
	dn4->GetComponent<CTransformComponent>()->Scale({100.0f,100.0f,100.0f});

	CGameObject* dn_2 = new CGameObject(1340);
	dn_2->AddComponent<CModelComponent>(*dn_2, ASSETPATH("Assets/Graphics/Exempel_Modeller/DetailNormals/Tufted_Leather/tufted_leather_dn.fbx"));
	dn_2->GetComponent<CTransformComponent>()->Position({5.0f,0.0f,0.0f});
	dn_2->GetComponent<CTransformComponent>()->Scale({100.0f,100.0f,100.0f});

	CGameObject* dn4_2 = new CGameObject(1341);
	dn4_2->AddComponent<CModelComponent>(*dn4_2, ASSETPATH("Assets/Graphics/Exempel_Modeller/DetailNormals/4DN/4DNs_dn.fbx"));
	dn4_2->GetComponent<CTransformComponent>()->Position({3.0f,0.0f,0.0f});
	dn4_2->GetComponent<CTransformComponent>()->Scale({100.0f,100.0f,100.0f});

	scene->AddInstance(dn);
	scene->AddInstance(dn4);
	scene->AddInstance(dn_2);
	scene->AddInstance(dn4_2);	
}

#include "animationLoader.h"// <-- include f�r AnimationLoader funktioner: AddAnimationsToGameObject() osv
void TEMP_SetUpAnimationTest(CScene* aScene)
{
	CGameObject* go3 = new CGameObject(123123125);	
	//std::string bugTestModel = "Assets/Temp/Robot/CH_E_Robot_SK.fbx"; 
	//std::string bugTestModel = "Assets/Temp/JointOrient/SK.fbx"; 
	std::string bugTestModel = "Assets/Temp/Main Character/CH_PL_SK.fbx"; 
	
	go3->AddComponent<CModelComponent>(*go3, bugTestModel);
	go3->myTransform->Position({ 0.0f,0.f,0.f });
	CAnimationComponent* animComp2 = AnimationLoader::AddAnimationsToGameObject(go3, bugTestModel);
	animComp2->BlendToAnimation(1, 1.0f);
	aScene->AddInstance(go3);
	g_TempAnimObject = go3;

	CGameObject* wall = new CGameObject(1341);
	wall->AddComponent<CModelComponent>(*wall, ASSETPATH("Assets/Graphics/Exempel_Modeller/Wall/Wall.fbx"));
	wall->GetComponent<CTransformComponent>()->Position({3.0f,0.0f,0.0f});
	aScene->AddInstance(wall);
}
#define GetAnimComp g_TempAnimObject->GetComponent<CAnimationComponent>()
void TEMP_AnimObjectControl()
{
	// Steps animation roughly??? 1 frame? Maybe idk. Should atleast. (Aki)
#ifdef ANIMATION_DEBUG
	if (Input::GetInstance()->IsKeyPressed(VK_LEFT))
	{
		GetAnimComp->StepAnimation(-1.0f);
	}
	if (Input::GetInstance()->IsKeyPressed(VK_RIGHT))
	{
		GetAnimComp->StepAnimation(1.0f);
	}
#endif

	float blend = 1.0f;// >1.0f is scary
	if (Input::GetInstance()->IsKeyPressed('1'))
	{
		GetAnimComp->BlendToAnimation(1, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('2'))
	{
		GetAnimComp->BlendToAnimation(2, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('3'))
	{
		GetAnimComp->BlendToAnimation(3, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('4'))
	{
		GetAnimComp->BlendToAnimation(4, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('5'))
	{
		GetAnimComp->BlendToAnimation(5, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('6'))
	{
		GetAnimComp->BlendToAnimation(6, blend);
	}
	if (Input::GetInstance()->IsKeyPressed('7'))
	{
		GetAnimComp->BlendToAnimation(7, blend);
	}

	if (Input::GetInstance()->IsKeyPressed(VK_F2))
	{
		//GetAnimComp->BlendLerpBetween(4, 5, 0.0f);
		GetAnimComp->BlendLerpBetween(1, 2, 0.0f);
	}
	//if (Input::GetInstance()->IsKeyPressed(VK_F3))
	//{
	//	GetAnimComp->BlendLerpBetween(1, 3, 0.0f);
	//}
	//if (Input::GetInstance()->IsKeyPressed(VK_F4))
	//{
	//	GetAnimComp->BlendLerpBetween(2, 6, 0.0f);
	//}

	static float blendLerp = 0.0f;
	if (Input::GetInstance()->IsKeyPressed('9'))
	{
		GetAnimComp->BlendLerp(blendLerp += 0.1f);
		std::cout << __FUNCTION__ << " blendLerp @ " << blendLerp << std::endl;
	}
	if (Input::GetInstance()->IsKeyPressed('0'))
	{
		GetAnimComp->BlendLerp(blendLerp -= 0.1f);
		std::cout << __FUNCTION__ << " blendLerp @ " << blendLerp << std::endl;
	}
}

void CInGameState::TEMP_DecalTests(CScene* aScene)
{
	CGameObject* decal = new CGameObject(20000);
	decal->AddComponent<CDecalComponent>(*decal, "Alpha");
	decal->GetComponent<CTransformComponent>()->Position({ 29.0f,2.0f, 0.0f });
	//decal->myTransform->Rotation({ 90.0f, 0.0f, 0.0f });
	//decal->GetComponent<CTransformComponent>()->Position({ 33.0f,2.0f, 25.5f });
	//decal->GetComponent<CTransformComponent>()->Position({ 14.0f, 1.0f, 20.0f });
	//decal->myTransform->Rotation({ 0.0f, 0.0f, 0.0f });
	//decal->myTransform->Scale({ 2.0f, 2.0f, 1.0f });
	myDecal = decal;
	aScene->AddInstance(decal);

	//CGameObject* decal2 = new CGameObject(20001);
	//decal2->AddComponent<CDecalComponent>(*decal2, "Alpha");
	//decal2->GetComponent<CTransformComponent>()->Position({ 12.0f, 1.5f, 20.0f });
	//decal2->myTransform->Rotation({ 0.0f, 45.0f, 0.0f });
	//decal2->myTransform->Scale({ 1.0f, 1.0f, 1.0f });
	//aScene->AddInstance(decal2);
}