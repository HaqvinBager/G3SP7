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
	camera->AddComponent<CCameraComponent>(*camera, 70.0f);
	camera->AddComponent<CCameraControllerComponent>(*camera, 2.0f);
	camera->myTransform->Position({ 0.0f, 1.0f, 0.0f });
	camera->myTransform->Rotation({ 0.0f, 0.0f, 0.0f });

	CGameObject* envLight = new CGameObject(1);
	envLight->AddComponent<CEnviromentLightComponent>(*envLight);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetIntensity(1.f);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetDirection({ 0.0f,1.0f,1.0f });

	CScene* emptyScene = new CScene(2);
	emptyScene->AddInstance(camera);
	emptyScene->MainCamera(camera->GetComponent<CCameraComponent>());
	emptyScene->EnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());
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