#include "stdafx.h"
#include "SceneManager.h"
#include "EnvironmentLight.h"
#include "CameraComponent.h"
#include "InstancedModelComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"
#include "ModelComponent.h"
//#include <PlayerControllerComponent.h>
#include "JsonReader.h"
#include "PointLightComponent.h"
#include "DecalComponent.h"
//#include <iostream>

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
	camera->myTransform->Rotation({ 0.0f, 0.0f, 0.0f });

	CGameObject* envLight = new CGameObject(1);
	envLight->AddComponent<CEnviromentLightComponent>(*envLight);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnvironmentLight()->SetIntensity(1.f);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnvironmentLight()->SetDirection({ 0.0f,1.0f,1.0f });
	//envLight->GetComponent<CEnviromentLightComponent>()->GetEnvironmentLight()->SetColor({ 1.0f, 0.0f, 0.0f });

	CScene* emptyScene = new CScene(2);
	emptyScene->AddInstance(camera);
	emptyScene->MainCamera(camera->GetComponent<CCameraComponent>());
	emptyScene->EnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnvironmentLight());
	emptyScene->AddInstance(envLight);

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
		int id = m["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(id);
		gameObject->AddComponent<CModelComponent>(*gameObject, ASSETPATH(CJsonReader::Get()->GetAssetPath(m["assetID"].GetInt())));
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

		gameObject->AddComponent<CInstancedModelComponent>(*gameObject, ASSETPATH(CJsonReader::Get()->GetAssetPath(assetID)), instancedModelTransforms);
		aScene.AddInstance(gameObject);
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


		//CDecalComponent* component = 
		//component->SetAlphaThreshold(decal["alphaThreshold"].GetFloat());
