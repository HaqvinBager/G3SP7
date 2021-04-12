#include "stdafx.h"
#include "SceneManager.h"
#include "EnvironmentLight.h"
#include "CameraComponent.h"
#include "InstancedModelComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"
#include "ModelComponent.h"

#include "PointLightComponent.h"
#include "DecalComponent.h"
#include "Engine.h"
#include "Scene.h"
#include "RigidBodyComponent.h"
#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "CapsuleColliderComponent.h"
#include "ConvexMeshColliderComponent.h"
#include <GravityGloveComponent.h>
#include <EnemyComponent.h>
//#include <iostream>

#include <BinReader.h>
#include <PlayerControllerComponent.h>
#include <AIController.h>
#include <PlayerComponent.h>
#include "animationLoader.h"
#include "AnimationComponent.h"


#include <ppl.h>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>


CScene* CSceneManager::ourLastInstantiatedScene = nullptr;
CSceneManager::CSceneManager()
{
}

CSceneManager::~CSceneManager()
{
}

CScene* CSceneManager::CreateEmpty()
{
	CScene* emptyScene = Instantiate();

	CGameObject* camera = new CGameObject(0);
	camera->AddComponent<CCameraComponent>(*camera);//Default Fov is 70.0f
	camera->AddComponent<CCameraControllerComponent>(*camera); //Default speed is 2.0f
	camera->myTransform->Position({ 0.0f, 1.0f, 0.0f });

	CGameObject* envLight = new CGameObject(1);
	envLight->AddComponent<CEnvironmentLightComponent>(*envLight, "Cubemap_Inside");
	envLight->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight()->SetIntensity(0.f);
	envLight->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight()->SetColor({ 0.f, 0.f, 0.f });
	envLight->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight()->SetDirection({ 0.0f,1.0f,1.0f });

	emptyScene->AddInstance(camera);
	emptyScene->MainCamera(camera->GetComponent<CCameraComponent>());
	emptyScene->EnvironmentLight(envLight->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight());
	emptyScene->AddInstance(envLight);

	//AddPlayer(*emptyScene, std::string());

	return emptyScene;
}

CScene* CSceneManager::CreateScene(const std::string& aSceneJson)
{
	CScene* scene = Instantiate();

	//SSceneSetup sceneSetup = {};
	//auto iter = sceneSetup.myGameObjects.push_back(new CGameObject(0));
	//(*iter)->AddComponent<CCameraComponent>();
	//CScene* scene = Instantiate();

	//CGameObject* camera = new CGameObject(0);
	//camera->AddComponent<CCameraComponent>(*camera);//Default Fov is 70.0f
	//camera->AddComponent<CCameraControllerComponent>(*camera); //Default speed is 2.0f
	//camera->myTransform->Position({ 0.0f, 1.0f, 0.0f });

	//scene->AddInstance(camera);
	//scene->MainCamera(camera->GetComponent<CCameraComponent>());

	const auto doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aSceneJson + "/" + aSceneJson + ".json"));
	if (doc.HasParseError())
		return nullptr;

	if (!doc.HasMember("Root"))
		return nullptr;

	SVertexPaintCollection vertexPaintData = CBinReader::LoadVertexPaintCollection(doc["Root"].GetString());
	const auto& scenes = doc.GetObjectW()["Scenes"].GetArray();
	for (const auto& sceneData : scenes)
	{
		if (AddGameObjects(*scene, sceneData["Ids"].GetArray()))
		{
			SetTransforms(*scene, sceneData["transforms"].GetArray());
			AddDirectionalLight(*scene, sceneData["directionalLight"].GetObjectW());
			AddPointLights(*scene, sceneData["lights"].GetArray());
			AddModelComponents(*scene, sceneData["models"].GetArray());
			SetVertexPaintedColors(*scene, sceneData["vertexColors"].GetArray(), vertexPaintData);
			AddDecalComponents(*scene, sceneData["decals"].GetArray());
			AddCollider(*scene, sceneData["colliders"].GetArray());
			AddEnemyComponents(*scene, sceneData["enemies"].GetArray());
		}
		AddInstancedModelComponents(*scene, sceneData["instancedModels"].GetArray());
	}

	CEngine::GetInstance()->GetPhysx().Cooking(scene->ActiveGameObjects(), scene);
	AddPlayer(*scene); //This add player does not read data from unity. (Yet..!) /Axel 2021-03-24

	scene->InitCanvas(ASSETPATH("Assets/Graphics/UI/JSON/UI_HUD.json"));

	return scene;
}

CScene* CSceneManager::CreateMenuScene(const std::string& aSceneName, const std::string& aCanvasPath)
{
	aSceneName;
	CScene* scene = CreateEmpty();
	scene->MainCamera()->GameObject().GetComponent<CCameraControllerComponent>()->SetCameraMode(CCameraControllerComponent::ECameraMode::MenuCam);
	scene->InitCanvas(aCanvasPath);

	return scene;
}

CScene* CSceneManager::Instantiate()
{
	// Depending on the order in which we Awake() different states/ scenes are created some states/scenes that should not listen to ComponentAdded might do so and will cause issues. / Aki 2021 04 08
	if (ourLastInstantiatedScene != nullptr)
		CMainSingleton::PostMaster().Unsubscribe(EMessageType::ComponentAdded, ourLastInstantiatedScene);

	ourLastInstantiatedScene = new CScene(); //Creates a New scene and Leaves total ownership of the Previous scene over to the hands of Engine!
	CMainSingleton::PostMaster().Subscribe(EMessageType::ComponentAdded, ourLastInstantiatedScene);
	return ourLastInstantiatedScene;
}

bool CSceneManager::AddGameObjects(CScene& aScene, RapidArray someData)
{
	if (someData.Size() == 0)
		return false;

	for (const auto& jsonID : someData)
	{
		int instanceID = jsonID.GetInt();
		aScene.AddInstance(new CGameObject(instanceID));
	}
	return true;
}

void CSceneManager::SetTransforms(CScene& aScene, RapidArray someData)
{
	for (const auto& t : someData)
	{
		int id = t["instanceID"].GetInt();
		CTransformComponent* transform = aScene.FindObjectWithID(id)->myTransform;
		transform->Scale({ t["scale"]["x"].GetFloat(),
							  t["scale"]["y"].GetFloat(),
							  t["scale"]["z"].GetFloat() });
		transform->Position({ t["position"]["x"].GetFloat(),
							  t["position"]["y"].GetFloat(),
							  t["position"]["z"].GetFloat() });
		transform->Rotation({ t["rotation"]["x"].GetFloat(),
							  t["rotation"]["y"].GetFloat(),
							  t["rotation"]["z"].GetFloat() });
	}
}

void CSceneManager::AddModelComponents(CScene& aScene, RapidArray someData)
{
	for (const auto& m : someData)
	{
		const int instanceId = m["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(instanceId);
		if (!gameObject)
			continue;

		const int assetId = m["assetID"].GetInt();
		if (CJsonReader::Get()->HasAssetPath(assetId))
		{
			gameObject->AddComponent<CModelComponent>(*gameObject, ASSETPATH(CJsonReader::Get()->GetAssetPath(assetId)));
		}
	}
}

void CSceneManager::SetVertexPaintedColors(CScene& aScene, RapidArray someData, const SVertexPaintCollection& vertexColorData)
{
	for (const auto& i : someData)
	{
		std::vector<std::string> materials;
		materials.reserve(3);
		for (const auto& material : i["materialNames"].GetArray())
		{
			materials.push_back(material.GetString());
		}

		int vertexColorID = i["vertexColorsID"].GetInt();
		for (const auto& gameObjectID : i["instanceIDs"].GetArray())
		{
			CGameObject* gameObject = aScene.FindObjectWithID(gameObjectID.GetInt());
			if (gameObject == nullptr) //Was not entirely sure why this would try to acces a non-existant gameObject.
				continue;				//I think it relates to the Model or VertexPaint Export. We don't want to add vertexColors to the export if said object was never painted on!



			for (auto it = vertexColorData.myData.begin(); it != vertexColorData.myData.end(); ++it)
			{
				if ((*it).myVertexMeshID == vertexColorID)
				{
					gameObject->GetComponent<CModelComponent>()->InitVertexPaint(it, materials);
				}
			}
		}
	}
}

void CSceneManager::AddInstancedModelComponents(CScene& aScene, RapidArray someData)
{
	for (const auto& i : someData)
	{
		int assetID = i["assetID"].GetInt();
		CGameObject* gameObject = new CGameObject(assetID);
		gameObject->IsStatic(true);
		std::vector<Matrix> instancedModelTransforms;
		instancedModelTransforms.reserve(i["transforms"].GetArray().Size());

		for (const auto& t : i["transforms"].GetArray())
		{
			CGameObject temp(0);
			CTransformComponent transform(temp);
			transform.Scale({ t["scale"]["x"].GetFloat(),
								 t["scale"]["y"].GetFloat(),
								 t["scale"]["z"].GetFloat() });
			transform.Position({ t["position"]["x"].GetFloat(),
								 t["position"]["y"].GetFloat(),
								 t["position"]["z"].GetFloat() });
			transform.Rotation({ t["rotation"]["x"].GetFloat(),
								 t["rotation"]["y"].GetFloat(),
								 t["rotation"]["z"].GetFloat() });
			instancedModelTransforms.emplace_back(transform.GetLocalMatrix());
		}
		if (CJsonReader::Get()->HasAssetPath(assetID))
		{
			gameObject->AddComponent<CInstancedModelComponent>(*gameObject, ASSETPATH(CJsonReader::Get()->GetAssetPath(assetID)), instancedModelTransforms);
			aScene.AddInstance(gameObject);
		}
	}
}

void CSceneManager::AddDirectionalLight(CScene& aScene, RapidObject someData)
{
	const auto& id = someData["instanceID"].GetInt();

	if (id == 0)
		return;

	CGameObject* gameObject = aScene.FindObjectWithID(id);
	if (gameObject == nullptr)
		return;

	gameObject->AddComponent<CEnvironmentLightComponent>(
		*gameObject,
		someData["cubemapName"].GetString(),
		Vector3(someData["r"].GetFloat(),
		someData["g"].GetFloat(),
		someData["b"].GetFloat()),
		someData["intensity"].GetFloat(),
		Vector3(someData["direction"]["x"].GetFloat(),
		someData["direction"]["y"].GetFloat(),
		someData["direction"]["z"].GetFloat())
		);
	aScene.EnvironmentLight(gameObject->GetComponent<CEnvironmentLightComponent>()->GetEnvironmentLight());
}

void CSceneManager::AddPointLights(CScene& aScene, RapidArray someData)
{
	for (const auto& pointLight : someData)
	{
		const auto& id = pointLight["instanceID"].GetInt();

		CGameObject* gameObject = aScene.FindObjectWithID(id);
		if (gameObject == nullptr)
			continue;

		CPointLightComponent* pointLightComponent = gameObject->AddComponent<CPointLightComponent>(
			*gameObject,
			pointLight["range"].GetFloat(),
			Vector3(pointLight["r"].GetFloat(),
			pointLight["g"].GetFloat(),
			pointLight["b"].GetFloat()),
			pointLight["intensity"].GetFloat());
		aScene.AddInstance(pointLightComponent->GetPointLight());
	}
}

void CSceneManager::AddDecalComponents(CScene& aScene, RapidArray someData)
{
	for (const auto& decal : someData)
	{
		CGameObject* gameObject = aScene.FindObjectWithID(decal["instanceID"].GetInt());
		gameObject->AddComponent<CDecalComponent>(*gameObject, decal["materialName"].GetString());
	}
}

void CSceneManager::AddPlayer(CScene& aScene/*, RapidObject someData*/)
{
	/*CGameObject* player = nullptr;
	if (!someData.HasMember("instanceID"))
		return;

	const int instanceID = someData["instanceID"].GetInt();
	if (instanceID == 0)
	{*/
	/*}
	else
	{
		player = aScene.FindObjectWithID(instanceID);
	}*/

	CGameObject* player = new CGameObject(87);
	//if (player == nullptr)
	//	return;

	CGameObject* camera = CCameraControllerComponent::CreatePlayerFirstPersonCamera(player);//new CGameObject(96);
	CGameObject* model = new CGameObject(88);
	std::string modelPath = ASSETPATH("Assets/Graphics/Character/Main_Character/CH_PL_SK.fbx");
	model->AddComponent<CModelComponent>(*model, modelPath);
	model->myTransform->SetParent(camera->myTransform);
	model->myTransform->Rotation({ 0.0f, 0.0f, 0.0f });
	CAnimationComponent* animComp = AnimationLoader::AddAnimationsToGameObject(model, modelPath);
	animComp->BlendToAnimation(1);
	CGameObject* gravityGloveSlot = new CGameObject(99);
	gravityGloveSlot->myTransform->Scale(0.1f);
	gravityGloveSlot->myTransform->SetParent(camera->myTransform);
	gravityGloveSlot->myTransform->Position({ 0.f, 0.f, 1.5f });
	//std::string gravitytestpath = ASSETPATH("Assets/Graphics/Environmentprops/Static_props/EN_P_Tetrapod.fbx");
	//gravityGloveSlot->AddComponent<CModelComponent>(*gravityGloveSlot, gravitytestpath);
	camera->AddComponent<CGravityGloveComponent>(*camera, gravityGloveSlot->myTransform);
	player->AddComponent<CPlayerComponent>(*player);

	player->AddComponent<CPlayerControllerComponent>(*player);// CPlayerControllerComponent constructor sets position of camera child object.
	player->GetComponent<CPlayerControllerComponent>()->SetControllerPosition({ 0.f, 5.0f,0.0f });
	aScene.AddInstance(player);
	aScene.AddInstance(model);
	aScene.AddInstance(camera);
	aScene.AddInstance(gravityGloveSlot);
	aScene.MainCamera(camera->GetComponent<CCameraComponent>());
	aScene.Player(player);
}

void CSceneManager::AddEnemyComponents(CScene& aScene, RapidArray someData)
{
	for (const auto& m : someData)
	{
		const int instanceId = m["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(instanceId);
		if (!gameObject)
			continue;

		SEnemySetting settings;
		settings.myDistance = m["distance"].GetFloat();
		settings.myRadius = m["radius"].GetFloat();
		settings.mySpeed = m["speed"].GetFloat();
		settings.myHealth = m["health"].GetFloat();
		gameObject->AddComponent<CEnemyComponent>(*gameObject, settings);
	}
}

void CSceneManager::AddCollider(CScene& aScene, RapidArray someData)
{
	//const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFileName));
	//if (!CJsonReader::IsValid(doc, { "colliders" }))
	//	return;

	//const auto& colliders = doc.GetObjectW()["colliders"].GetArray();
	for (const auto& c : someData)
	{
		int id = c["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(id);

		ColliderType colliderType = static_cast<ColliderType>(c["colliderType"].GetInt());
		bool isStatic = c.HasMember("isStatic") ? c["isStatic"].GetBool() : false;

		CRigidBodyComponent* rigidBody = gameObject->GetComponent<CRigidBodyComponent>();
		if (rigidBody == nullptr && isStatic == false)
		{
			float mass = c["mass"].GetFloat();
			Vector3 localCenterMass;
			localCenterMass.x = c["localMassPosition"]["x"].GetFloat();
			localCenterMass.y = c["localMassPosition"]["y"].GetFloat();
			localCenterMass.z = c["localMassPosition"]["z"].GetFloat();
			Vector3 inertiaTensor;
			inertiaTensor.x = c["inertiaTensor"]["x"].GetFloat();
			inertiaTensor.y = c["inertiaTensor"]["y"].GetFloat();
			inertiaTensor.z = c["inertiaTensor"]["z"].GetFloat();
			gameObject->AddComponent<CRigidBodyComponent>(*gameObject, mass, localCenterMass, inertiaTensor);
		}

		Vector3 posOffset;
		posOffset.x = c["positionOffest"]["x"].GetFloat();
		posOffset.y = c["positionOffest"]["y"].GetFloat();
		posOffset.z = c["positionOffest"]["z"].GetFloat();

		float dynamicFriction = c["dynamicFriction"].GetFloat();
		float staticFriction = c["staticFriction"].GetFloat();
		float bounciness = c["bounciness"].GetFloat();

		switch (colliderType)
		{
		case ColliderType::BoxCollider:
			{
				Vector3 boxSize;
				boxSize.x = c["boxSize"]["x"].GetFloat();
				boxSize.y = c["boxSize"]["y"].GetFloat();
				boxSize.z = c["boxSize"]["z"].GetFloat();
				CBoxColliderComponent* boxCollider = gameObject->AddComponent<CBoxColliderComponent>(*gameObject, posOffset, boxSize, isStatic, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(dynamicFriction, staticFriction, bounciness));
				if (c.HasMember("isTrigger"))
					boxCollider->IsTrigger(c["isTrigger"].GetBool());
			}
			break;
		case ColliderType::SphereCollider:
			{
				float radius = c["sphereRadius"].GetFloat();
				gameObject->AddComponent<CSphereColliderComponent>(*gameObject, posOffset, radius, isStatic, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(dynamicFriction, staticFriction, bounciness));
			}
			break;
		case ColliderType::CapsuleCollider:
			{
				float radius = c["capsuleRadius"].GetFloat();
				float height = c["capsuleHeight"].GetFloat();
				gameObject->AddComponent<CCapsuleColliderComponent>(*gameObject, posOffset, radius, height, isStatic, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(dynamicFriction, staticFriction, bounciness));
			}
			break;
		case ColliderType::MeshCollider:
			{
				gameObject->AddComponent<CConvexMeshColliderComponent>(*gameObject, CEngine::GetInstance()->GetPhysx().CreateCustomMaterial(dynamicFriction, staticFriction, bounciness));
			}
			break;
		}
	}
}

CSceneFactory* CSceneFactory::ourInstance = nullptr;
CSceneFactory::CSceneFactory()
{
	ourInstance = this;
}

CSceneFactory::~CSceneFactory()
{
	ourInstance = nullptr;
}

CSceneFactory* CSceneFactory::Get()
{
	return ourInstance;
}

void CSceneFactory::LoadScene(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete)
{
	CScene* loadedScene = CSceneManager::CreateScene(aSceneName);

	CEngine::GetInstance()->AddScene(aState, loadedScene);
	CEngine::GetInstance()->SetActiveScene(aState);

	if (onComplete != nullptr)
		onComplete(aSceneName);
}

void CSceneFactory::LoadSceneAsync(const std::string& aSceneName, const CStateStack::EState aState, std::function<void(std::string)> onComplete)
{
	myOnComplete = onComplete;
	myLastSceneName = aSceneName;
	myLastLoadedState = aState;
	myFuture = std::async(std::launch::async, &CSceneManager::CreateScene, aSceneName);
}

void CSceneFactory::Update()
{
	if (myFuture._Is_ready())
	{
		CScene* loadedScene = myFuture.get();
		if (loadedScene == nullptr)
		{
			ENGINE_ERROR_MESSAGE("Failed to Load Scene %s", myLastSceneName.c_str());
			return;
		}

		CEngine::GetInstance()->AddScene(myLastLoadedState, loadedScene);
		CEngine::GetInstance()->SetActiveScene(myLastLoadedState);
		myOnComplete(myLastSceneName);
	}
}

void CSceneFactory::Transition(const std::string& aFromScene, const std::string& aToScene, const std::string& aTransitionScene)
{
	aTransitionScene;
	aToScene;
	aFromScene;
	//myTransitionThread = std::async(std::launch::async, &CSceneFactory::FillSceneAsync, aToScene, [this] () { OnTransitionComplete(); });

}

CSceneSetup CSceneFactory::FillSceneAsync(const std::string& aSceneName, std::function<void()> /*onComplete*/)
{
	aSceneName;
	//OnComplete;
	CSceneSetup setup;

	return setup;
}

void CSceneFactory::OnTransitionComplete()
{

}