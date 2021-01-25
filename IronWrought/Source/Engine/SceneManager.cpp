#include "stdafx.h"
#include "SceneManager.h"
#include "JsonReader.h"
#include "EnvironmentLight.h"
#include "CameraComponent.h"
#include "InstancedModelComponent.h"
#include "CameraControllerComponent.h"
#include "EnviromentLightComponent.h"



CSceneManager::CSceneManager()
{
}

CSceneManager::~CSceneManager()
{
}

CScene* CSceneManager::CreateScene(std::vector<std::string> aJsonFile)
{
		CScene* scene = new CScene();
	for (int i = 0; i < aJsonFile.size(); ++i) {
		rapidjson::Document document = CJsonReader::LoadDocument(ASSETPATH + "Assets/Generated/" + aJsonFile[i]);
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
				instancedTransforms.emplace_back(transform.GetMatrix());

			}

			instancedGameObject->AddComponent<CInstancedModelComponent>(*instancedGameObject, std::string(ASSETPATH + model_path), instancedTransforms);
			scene->AddInstance(instancedGameObject);
		}
		scene->AddPXScene(CMainSingleton::PhysXWrapper().CreatePXScene());

		CGameObject* camera = new CGameObject(0);
		camera->AddComponent<CCameraComponent>(*camera, 70.0f);
		camera->AddComponent<CCameraControllerComponent>(*camera, 25.0f);
		camera->myTransform->Position({ 0.0f, 1.0f, 0.0f });
		camera->myTransform->Rotation({ 0.0f, 0.0f, 0.0f });
		scene->AddInstance(camera);
		scene->SetMainCamera(camera->GetComponent<CCameraComponent>());

		CGameObject* envLight = new CGameObject(1);
		envLight->AddComponent<CEnviromentLightComponent>(*envLight);
		envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetIntensity(1.f);
		envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight()->SetDirection({ 0.0f,0.0f,-1.0f });
		scene->AddInstance(envLight);
		scene->SetEnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());
		
	}
	return scene;
}
