#include "stdafx.h"
#include "SceneManager.h"
#include "EnvironmentLight.h"
#include "CameraComponent.h"
#include "InstancedModelComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"
#include "ModelComponent.h"
#include <iostream>
#include <PlayerControllerComponent.h>


std::string CSceneManager::npos = "";
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


CScene* CSceneManager::CreateScene(const std::vector<std::string>& someJsonFiles)
{
	CScene* scene = CreateEmpty();
	AddGameObjects(*scene, someJsonFiles);
	SetTransforms(*scene, someJsonFiles);
	AddModelComponents(*scene, someJsonFiles);
	AddInstancedModelComponents(*scene, someJsonFiles);
	return scene;
}

bool CSceneManager::AddGameObjects(CScene& aScene, const std::vector<std::string>& someJsonFiles)
{
	const std::string& jsonPath = Find(someJsonFiles, "InstanceIDCollection");//*jsonPathIterator;
	if (jsonPath.size() == 0)
		return false;

	const auto& idDoc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + jsonPath));
	if (!IsValid(idDoc))
		return false;

	const auto& idArray = idDoc.GetObjectW()["Ids"].GetArray();
	for (const auto& id : idArray) {
		int instanceID = id.GetInt();
		aScene.AddInstance(new CGameObject(instanceID));
	}
	return true;
}

void CSceneManager::SetTransforms(CScene& aScene, const std::vector<std::string>& someJsonFiles)
{

	const std::string& jsonPath = Find(someJsonFiles, "TransformCollection");//*jsonPathIterator;
	if (jsonPath.size() == 0)
		return;

	const auto& transformDoc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + jsonPath));
	const auto& transformArray = transformDoc.GetObjectW()["transforms"].GetArray();
	for (const auto& t : transformArray) {
		int id = t["instanceID"].GetInt();
		CTransformComponent* transform = aScene.FindObjectWithID(id)->myTransform;
		transform->Scale	({t["scale"]["x"].GetFloat(),
							  t["scale"]["y"].GetFloat(),
							  t["scale"]["z"].GetFloat() });
		transform->Position	({t["position"]["x"].GetFloat(),
							  t["position"]["y"].GetFloat(),
							  t["position"]["z"].GetFloat() });
		transform->Rotation	({t["rotation"]["x"].GetFloat(),
							  t["rotation"]["y"].GetFloat(),
							  t["rotation"]["z"].GetFloat() });
	}
}

void CSceneManager::AddModelComponents(CScene& aScene, const std::vector<std::string>& someJsonFiles)
{
	const std::string& jsonPath = Find(someJsonFiles, "ModelCollection");//*jsonPathIterator;
	if (jsonPath.size() == 0)
		return;

	const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + jsonPath));
	if (!doc.HasMember("modelLinks"))
	{
		return;
	}
	const auto& modelArray = doc.GetObjectW()["modelLinks"].GetArray();
	
	for (const auto& m : modelArray) {
		int id = m["instanceID"].GetInt();
		CGameObject* gameObject = aScene.FindObjectWithID(id);
		gameObject->AddComponent<CModelComponent>(*gameObject, ASSETPATH(CJsonReader::Get()->GetAssetPath(m["assetID"].GetInt())));
	}
}

void CSceneManager::AddInstancedModelComponents(CScene& aScene, const std::vector<std::string>& someJsonFiles)
{
	const std::string& jsonPath = Find(someJsonFiles, "InstanceModelCollection");//*jsonPathIterator;
	if (jsonPath.size() == 0)
		return;

	const auto& doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + jsonPath));
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

const std::string& CSceneManager::Find(const std::vector<std::string>& someStrings, const std::string& aContains)
{
	auto jsonPathIterator = std::find_if(someStrings.begin(), someStrings.end(), [&](std::string e) { return e.find(aContains) != std::string::npos; });

	if (jsonPathIterator == someStrings.end())
		return npos;

	return *jsonPathIterator;
}


CScene* CSceneManager::CreateScene(std::string aJsonFile)//TEMP
{
	CScene* scene = new CScene(1);
	rapidjson::Document document = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFile));
	if (document.HasMember("instancedGameobjects")) {


		auto jsonarray = document["instancedGameobjects"].GetArray();
		for (auto& jsongameobject : jsonarray) {

			CGameObject* instancedGameObject = new CGameObject(0);
			std::string model_path;
			//float instanceID;
			Vector3 position;
			Vector3 rotation;
			Vector3 scale;

			auto jsonmodelpath = jsongameobject["model"].GetObjectW();
			model_path = jsonmodelpath["fbxPath"].GetString();
			auto jsonTransforms = jsongameobject["transforms"].GetArray();
			std::vector<DirectX::SimpleMath::Matrix> instancedTransforms;
			for (auto& jsonTransform : jsonTransforms) {
				//auto jsoninstanceID = jsontransform["instanceID"].GetObjectW();
				auto jsonposition = jsonTransform["position"].GetObjectW();
				auto jsonrotation = jsonTransform["rotation"].GetObjectW();
				auto jsonscale = jsonTransform["scale"].GetObjectW();

				//instanceID = jsoninstanceID[""].GetFloat();

				position.x = jsonposition["x"].GetFloat();
				position.y = jsonposition["y"].GetFloat();
				position.z = jsonposition["z"].GetFloat();

				rotation.x = jsonrotation["x"].GetFloat();
				rotation.y = jsonrotation["y"].GetFloat();
				rotation.z = jsonrotation["z"].GetFloat();

				scale.x = jsonscale["x"].GetFloat();
				scale.y = jsonscale["y"].GetFloat();
				scale.z = jsonscale["z"].GetFloat();

				CGameObject temp(0);
				CTransformComponent transform(temp);
				transform.Scale(scale);
				transform.Position(position);
				transform.Rotation(rotation);
				instancedTransforms.emplace_back(transform.GetLocalMatrix());
			}
			instancedGameObject->AddComponent<CInstancedModelComponent>(*instancedGameObject, ASSETPATH(model_path), instancedTransforms);
			scene->AddInstance(instancedGameObject);
		}
	}

	if (document.HasMember("modelGameObjects")) {
		auto jsonGameObjectArray = document["modelGameObjects"].GetArray();
		for (const auto& jsonModelGameObject : jsonGameObjectArray) {
			if (jsonModelGameObject.HasMember("transform")) {
				const auto& jsonTransform = jsonModelGameObject["transform"].GetObjectW();

				auto jsonposition = jsonTransform["position"].GetObjectW();
				auto jsonrotation = jsonTransform["rotation"].GetObjectW();
				auto jsonscale = jsonTransform["scale"].GetObjectW();

				int instanceID = jsonTransform["instanceID"].GetInt();

				Vector3 position = {};
				position.x = jsonposition["x"].GetFloat();
				position.y = jsonposition["y"].GetFloat();
				position.z = jsonposition["z"].GetFloat();

				Vector3 rotation = {};
				rotation.x = jsonrotation["x"].GetFloat();
				rotation.y = jsonrotation["y"].GetFloat();
				rotation.z = jsonrotation["z"].GetFloat();

				Vector3 scale = {};
				scale.x = jsonscale["x"].GetFloat();
				scale.y = jsonscale["y"].GetFloat();
				scale.z = jsonscale["z"].GetFloat();

				if (jsonModelGameObject.HasMember("model")) {
					const auto& jsonModel = jsonModelGameObject["model"].GetObjectW();
					std::string modelPath = jsonModel["fbxPath"].GetString();
					CGameObject* modelGameObject = new CGameObject(instanceID);
					modelGameObject->AddComponent<CModelComponent>(*modelGameObject, ASSETPATH(modelPath));
					modelGameObject->myTransform->Scale(scale);
					modelGameObject->myTransform->Rotation(rotation);
					modelGameObject->myTransform->Position(position);

					scene->AddInstance(modelGameObject);
				}
			}
		}
	}

	//CGameObject* player = new CGameObject(999);
	//player->AddComponent<CModelComponent>(*player, ASSETPATH + "Assets/Graphics/Character/Main Character/CH_PL_SK.fbx");
	//player->AddComponent<CPlayerControllerComponent>(*player);
	//scene->AddInstance(player);

	CGameObject* camera = new CGameObject(1000);
	camera->AddComponent<CCameraComponent>(*camera, 65.0f);
	camera->AddComponent<CCameraControllerComponent>(*camera, 5.0f);
	//camera->myTransform->Position({ 0.0f, 1.6f, -0.2f });
	//player->myTransform->SetParent(camera->myTransform);
	//camera->myTransform->SetParent(player->myTransform);
	//camera->myTransform->Rotation({ 0.0f, 180.0f, 0.0f });

	scene->AddInstance(camera);
	scene->MainCamera(camera->GetComponent<CCameraComponent>());

	CGameObject* envLight = new CGameObject(1);
	envLight->AddComponent<CEnviromentLightComponent>(*envLight);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetIntensity(1.f);
	envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetDirection({ 0.0f,-1.0f,1.0f });
	scene->EnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());
	scene->AddInstance(envLight);

	return scene;
}


bool CSceneManager::IsValid(const rapidjson::Document& aDoc)
{
	if (aDoc.HasParseError() ||
		!aDoc.GetObjectW().HasMember("sceneName") ||
		!aDoc.GetObjectW().HasMember("sceneName") ||
		!aDoc.GetObjectW().HasMember("Ids") ||
		!aDoc.GetObjectW()["Ids"].IsArray())
	{
		return false;
	}

	return true;
}




/*CScene* CSceneManager::CreateScene(std::vector<std::string> aJsonFile)
{
	CScene* scene = new CScene();
	for (int i = 0; i < aJsonFile.size(); ++i) {
		if (aJsonFile[i].find("Level") == aJsonFile[i].npos)
		{
			continue;
		}

		rapidjson::Document document = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + aJsonFile[i]));
		auto jsonarray = document["instancedGameobjects"].GetArray();
		for (auto& jsongameobject : jsonarray) {

			CGameObject* instancedGameObject = new CGameObject(0);
			std::string model_path;
			//float instanceID;
			Vector3 position;
			Vector3 rotation;
			Vector3 scale;

			auto jsonmodelpath = jsongameobject["model"].GetObjectW();
			model_path = jsonmodelpath["fbxPath"].GetString();
			auto jsonTransforms = jsongameobject["transforms"].GetArray();
			std::vector<DirectX::SimpleMath::Matrix> instancedTransforms;
			for (auto& jsonTransform : jsonTransforms) {
				//auto jsoninstanceID = jsontransform["instanceID"].GetObjectW();
				auto jsonposition = jsonTransform["position"].GetObjectW();
				auto jsonrotation = jsonTransform["rotation"].GetObjectW();
				auto jsonscale = jsonTransform["scale"].GetObjectW();

				//instanceID = jsoninstanceID[""].GetFloat();

				position.x = jsonposition["x"].GetFloat();
				position.y = jsonposition["y"].GetFloat();
				position.z = jsonposition["z"].GetFloat();

				rotation.x = jsonrotation["x"].GetFloat();
				rotation.y = jsonrotation["y"].GetFloat();
				rotation.z = jsonrotation["z"].GetFloat();

				scale.x = jsonscale["x"].GetFloat();
				scale.y = jsonscale["y"].GetFloat();
				scale.z = jsonscale["z"].GetFloat();
				float pos_x = position.x;
				float pos_y = position.y;
				float pos_z = position.z;
				std::cout << " Object: " + model_path << std::endl;
				std::cout << " -This Objects x,y,z: (" << pos_x << "," << pos_y << "," << pos_z << ")" << std::endl;
				CGameObject temp(0);
				CTransformComponent transform(temp);
				transform.Scale(scale);
				transform.Position(position);
				transform.Rotation(rotation);
				instancedTransforms.emplace_back(transform.GetLocalMatrix());

			}

			instancedGameObject->AddComponent<CInstancedModelComponent>(*instancedGameObject, ASSETPATH(model_path), instancedTransforms);
			scene->AddInstance(instancedGameObject);
		}
		//scene->AddPXScene(CMainSingleton::PhysXWrapper().CreatePXScene());

		CGameObject* camera = new CGameObject(0);
		camera->AddComponent<CCameraComponent>(*camera, 70.0f);
		camera->AddComponent<CCameraControllerComponent>(*camera, 5.0f);
		camera->myTransform->Position({ 0.0f, 1.0f, 0.0f });
		camera->myTransform->Rotation({ 0.0f, 0.0f, 0.0f });
		scene->AddInstance(camera);
		scene->MainCamera(camera->GetComponent<CCameraComponent>());

		CGameObject* envLight = new CGameObject(1);
		envLight->AddComponent<CEnviromentLightComponent>(*envLight);
		envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetIntensity(1.f);
		envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetDirection({ 0.0f,1.0f,1.0f });
		scene->AddInstance(envLight);
		scene->EnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());

	}
	return scene;
}
*/
