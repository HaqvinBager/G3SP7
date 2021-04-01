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
//#include <iostream>

#include <BinReader.h>
#include <PlayerControllerComponent.h>
#include "animationLoader.h"
#include "AnimationComponent.h"

CSceneManager::CSceneManager()
{
}

CSceneManager::~CSceneManager()
{
}

CScene* CSceneManager::CreateEmpty()
{
	CGameObject* camera = new CGameObject(0);
	camera->AddComponent<CCameraComponent>(*camera);//Default Fov is 70.0f
	camera->AddComponent<CCameraControllerComponent>(*camera); //Default speed is 2.0f
	camera->myTransform->Position({ 0.0f, 1.0f, 0.0f });
	
	CGameObject* envLight = new CGameObject(1);
	envLight->AddComponent<CEnviromentLightComponent>(*envLight);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnvironmentLight()->SetIntensity(0.f);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnvironmentLight()->SetColor({ 0.f, 0.f, 0.f });
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnvironmentLight()->SetDirection({ 0.0f,1.0f,1.0f });

	CScene* emptyScene = new CScene(2);
	emptyScene->AddInstance(camera);
	emptyScene->MainCamera(camera->GetComponent<CCameraComponent>());
	emptyScene->EnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnvironmentLight());
	emptyScene->AddInstance(envLight);

	//AddPlayer(*emptyScene, std::string());

	return emptyScene;
}

CScene* CSceneManager::CreateScene(const std::string& aSceneJson)
{
	CScene* scene = CreateEmpty();

	const auto doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aSceneJson + "/" + aSceneJson + ".json"));
	if(doc.HasParseError())
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
			AddModelComponents(*scene, sceneData["models"].GetArray());
			SetVertexPaintedColors(*scene, sceneData["vertexColors"].GetArray(), vertexPaintData);
			AddPointLights(*scene, sceneData["lights"].GetArray());
			AddDecalComponents(*scene, sceneData["decals"].GetArray());
			AddInstancedModelComponents(*scene, sceneData["instancedModels"].GetArray());	
		}
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
	for (const auto& t : someData) {
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

	for (const auto& m : someData) {
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
	for (const auto& i : someData) {
		int assetID = i["assetID"].GetInt();
		CGameObject* gameObject = new CGameObject(assetID);
		gameObject->IsStatic(true);
		std::vector<Matrix> instancedModelTransforms;
		instancedModelTransforms.reserve(i["transforms"].GetArray().Size());

		for (const auto& t : i["transforms"].GetArray()) {
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

void CSceneManager::AddPointLights(CScene& aScene, RapidArray someData)
{
	for (const auto& pointLight : someData) {
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
	for (const auto& decal : someData) {
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

	player->AddComponent<CPlayerControllerComponent>(*player);// CPlayerControllerComponent constructor sets position of camera child object.
	player->GetComponent<CPlayerControllerComponent>()->SetControllerPosition({ 0.f, 5.0f,0.0f });
	aScene.AddInstance(player);
	aScene.AddInstance(model);
	aScene.AddInstance(camera);
	aScene.MainCamera(camera->GetComponent<CCameraComponent>());
	aScene.Player(player);
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

void CSceneFactory::LoadSceneAsync(const std::string& aSceneName, std::function<void(std::string)> onComplete)
{
	myOnComplete = onComplete;
	myLastSceneName = aSceneName;
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
		
		CEngine::GetInstance()->AddScene(CStateStack::EState::InGame, loadedScene);
		CEngine::GetInstance()->SetActiveScene(CStateStack::EState::InGame);
		myOnComplete(myLastSceneName);
	}
}
