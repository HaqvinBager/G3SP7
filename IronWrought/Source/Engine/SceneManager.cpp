#include "stdafx.h"
#include "SceneManager.h"
#include "EnvironmentLight.h"
#include "CameraComponent.h"
#include "InstancedModelComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"
#include "ModelComponent.h"

#include "JsonReader.h"
#include "PointLightComponent.h"
#include "DecalComponent.h"
#include "Engine.h"
#include "Scene.h"
//#include <iostream>

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

CScene* CSceneManager::CreateScene(const std::string& aSceneName)
{
	CScene* scene = CreateEmpty();

	AddInstancedModelComponents(*scene, aSceneName + "_InstanceModelCollection.json");
	if (AddGameObjects(*scene, aSceneName + "_InstanceIDCollection.json")) {
		SetTransforms(*scene, aSceneName + "_TransformCollection.json");
		AddModelComponents(*scene, aSceneName + "_ModelCollection.json");
		AddPointLights(*scene, aSceneName + "_PointLightCollection.json");
		AddDecalComponents(*scene, aSceneName + "_DecalCollection.json");
		
	}
	// Should be in the if case above. Later. When some bugs are fixed.
		AddPlayer(*scene, aSceneName + "_Player.json");
	CEngine::GetInstance()->GetPhysx().Cooking(scene->ActiveGameObjects(), scene);
	return scene;
}



bool CSceneManager::AddGameObjects(CScene& aScene, const std::string& aJsonFileName)
{
	const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFileName));
	if (!CJsonReader::IsValid(doc, { "Ids" }))
		return false;

	const auto& idArray = doc.GetObjectW()["Ids"].GetArray();

	if (idArray.Size() == 0)
		return false;

	for (const auto& id : idArray) {
		int instanceID = id.GetInt();
		aScene.AddInstance(new CGameObject(instanceID));
	}
	return true;
}

void CSceneManager::SetTransforms(CScene& aScene, const std::string& aJsonFileName)
{
	const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFileName));

	if (!CJsonReader::IsValid(doc, { "transforms" }))
		return;

	const auto& transformArray = doc["transforms"].GetArray();
	for (const auto& t : transformArray) {
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

void CSceneManager::AddModelComponents(CScene& aScene, const std::string& aJsonFileName)
{
	const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFileName));
	if (!CJsonReader::IsValid(doc, { "modelLinks" }))
		return;

	const auto& modelArray = doc.GetObjectW()["modelLinks"].GetArray();
	for (const auto& m : modelArray) {
		const int instanceId = m["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(instanceId);
		if (!gameObject)
			continue;

		const int assetId = m["assetID"].GetInt();
#ifdef _DEBUG
		if (CJsonReader::Get()->HasAssetPath(assetId))
		{
			SetConsoleColor(CONSOLE_GREEN);
			std::cout << aJsonFileName << " " << " has AssetID: " << assetId << ". " << __FUNCTION__ << "()" << std::endl;
			SetConsoleColor(CONSOLE_WHITE);
			gameObject->AddComponent<CModelComponent>(*gameObject, ASSETPATH(CJsonReader::Get()->GetAssetPath(assetId)));
		}
		else
		{
			SetConsoleColor(CONSOLE_RED);
			std::cout << aJsonFileName << " " << "does not have AssetID: " << assetId << ". " << __FUNCTION__  << "()" << std::endl;
			SetConsoleColor(CONSOLE_WHITE);
		}
#else
		if (CJsonReader::Get()->HasAssetPath(assetId))
			gameObject->AddComponent<CModelComponent>(*gameObject, ASSETPATH(CJsonReader::Get()->GetAssetPath(assetId)));
#endif

	}
}

void CSceneManager::AddInstancedModelComponents(CScene& aScene, const std::string& aJsonFileName)
{
	const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFileName));

	if (!CJsonReader::IsValid(doc, { "instancedModels" }))
		return;

	const auto& instancedModelArray = doc.GetObjectW()["instancedModels"].GetArray();
	for (const auto& i : instancedModelArray) {
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

void CSceneManager::AddPointLights(CScene& aScene, const std::string& aJsonFileName)
{
	const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFileName));

	if (!CJsonReader::IsValid(doc, { "lights" }))
		return;

	const auto& pointLightArray = doc["lights"].GetArray();
	for (const auto& pointLight : pointLightArray) {
		const auto& id = pointLight["instanceID"].GetInt();

		CGameObject* gameObject = aScene.FindObjectWithID(id);
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

void CSceneManager::AddDecalComponents(CScene& aScene, const std::string& aJsonFileName)
{
	const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFileName));
	if (!CJsonReader::IsValid(doc, { "links" }))
		return;

	const auto& idArray = doc.GetObjectW()["links"].GetArray();

	if (idArray.Size() == 0)
		return;

	for (const auto& decal : idArray) {
		CGameObject* gameObject = aScene.FindObjectWithID(decal["instanceID"].GetInt());
		gameObject->AddComponent<CDecalComponent>(*gameObject, decal["materialName"].GetString());
	}
}

void CSceneManager::AddPlayer(CScene& aScene, const std::string& aJsonFileName)
{
	CGameObject* player = nullptr;
	const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFileName));
	if (CJsonReader::IsValid(doc, { "instanceID" })) 
	{
		const int instanceID = doc["instanceID"].GetInt();
		player = aScene.FindObjectWithID(instanceID);
	}
	else 
	{
		player = new CGameObject(87);
	}

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
