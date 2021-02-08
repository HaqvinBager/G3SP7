#include "stdafx.h"
#include "UnityFactory.h"
#include "Scene.h"
#include "GameObject.h"

#include "Component.h"
#include "ModelComponent.h"
#include "InstancedModelComponent.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "EnviromentLightComponent.h"
#include "PointLightComponent.h"

#include "RectangleColliderComponent.h"
#include "CircleColliderComponent.h"
#include "CameraControllerComponent.h"//Could not compile 20201124 21:05 without this

#include "CollisionManager.h"
#include "LightFactory.h"
#include "PointLight.h"
#include "animationLoader.h"
#include "Model.h"

#include "Debug.h"

#define ENGINE_SCALE 0.01f

float Lerp(float a, float b, float t) {
	return a + t * (b - a);
}

float InverseLerp(float a, float b, float t) {
	return (t - a) / (b - a);
}


CUnityFactory::CUnityFactory()
{

}

CUnityFactory::~CUnityFactory()
{

}

//bool CUnityFactory::FillScene(const SLoadScreenData& aData, const std::vector<std::string>& /*someModelPaths*/, CScene& aScene)
//{
//	CGameObject* camera = CreateGameObject(aData.myCamera, false);
//	aScene.AddInstance(camera);
//	aScene.MainCamera(camera->GetComponent<CCameraComponent>());
//	CGameObject* envLight = CreateGameObject(aData.myDirectionalLight);
//	aScene.AddInstance(envLight);
//	aScene.EnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());
//	return true;
//}

//bool CUnityFactory::FillScene(const SInGameData& aData, const std::vector<std::string>& aBinModelPaths, CScene& aScene)
//{
//	CGameObject* camera = CreateGameObject(aData.myCamera);
//	aScene.AddInstance(camera);
//	aScene.MainCamera(camera->GetComponent<CCameraComponent>());
//
//	CGameObject* envLight = CreateGameObject(aData.myDirectionalLight);
//	aScene.AddInstance(envLight);
//	aScene.EnvironmentLight(envLight->GetComponent<CEnviromentLightComponent>()->GetEnviromentLight());
//
//	for (const auto& pointLightData : aData.myPointLightData)
//	{
//		CGameObject* pointLight = CreateGameObject(pointLightData);
//		aScene.AddInstance(pointLight);
//		aScene.AddInstance(pointLight->GetComponent<CPointLightComponent>()->GetPointLight());
//	}
//
//	std::unordered_map<int, int> modelIndexMap;
//	for (const auto& gameObjectData : aData.myGameObjects)
//	{
//		if (modelIndexMap.find(gameObjectData.myModelIndex) == modelIndexMap.end())
//		{
//			modelIndexMap[gameObjectData.myModelIndex] = 0;
//		}
//		modelIndexMap[gameObjectData.myModelIndex]++;
//	}
//
//	std::unordered_map<int, std::vector<Matrix>> transformIndexMap;
//	for (const auto& go : aData.myGameObjects)
//	{
//		if (transformIndexMap.find(go.myModelIndex) == transformIndexMap.end())
//		{
//			transformIndexMap[go.myModelIndex].reserve(modelIndexMap[go.myModelIndex]);
//		}
//		Matrix transform = { };
//		//Scale
//		Vector3 scale;
//		Vector3 translation;
//		Quaternion rotation;
//		transform.Decompose(scale, rotation, translation);
//		transform = Matrix::CreateFromQuaternion(rotation);
//		transform *= Matrix::CreateScale(go.myScale.x * ENGINE_SCALE);
//		transform.Translation(translation);
//
//		//Position
//		transform.Translation(go.myPosition);
//
//		//Rotation
//		Vector3 tempTranslation = transform.Translation();
//
//		DirectX::SimpleMath::Matrix tempRotation = Matrix::CreateFromYawPitchRoll(
//			DirectX::XMConvertToRadians(go.myRotation.y),
//			DirectX::XMConvertToRadians(go.myRotation.x),
//			DirectX::XMConvertToRadians(go.myRotation.z)
//		);
//
//		transform = tempRotation;
//		transform *= Matrix::CreateScale(go.myScale.x * ENGINE_SCALE);
//		transform.Translation(tempTranslation);
//
//
//		transformIndexMap[go.myModelIndex].emplace_back(transform);
//	}
//
//	for (int key = 0; key < aBinModelPaths.size(); ++key)
//	{
//		if (modelIndexMap.find(key) != modelIndexMap.end())
//		{
//			aScene.AddInstance(CreateGameObjectInstanced(aBinModelPaths[key], modelIndexMap[key], transformIndexMap[key]));
//		}
//	}
//
//	return true;
//}

//CGameObject* CUnityFactory::CreateGameObject(const SCameraData& aData, bool addCameraController)
//{
//	CGameObject* gameObject = new CGameObject(0);
//	auto camComponent = gameObject->AddComponent<CCameraComponent>(*gameObject, aData.myFieldOfView);
//	if (addCameraController)
//	{
//		int cameraMode = static_cast<int>(aData.myStartInCameraMode);
//		gameObject->AddComponent<CCameraControllerComponent>(*gameObject, aData.myFreeCamMoveSpeed, static_cast<CCameraControllerComponent::ECameraMode>(cameraMode), static_cast<char>(aData.myToggleFreeCamKey), aData.myOffset);
//	}
//	gameObject->myTransform->Position(aData.myPosition);
//	gameObject->myTransform->Rotation(aData.myRotation);
//	camComponent->SetStartingRotation(aData.myRotation);
//	return std::move(gameObject);
//}

//CGameObject* CUnityFactory::CreateGameObject(const SDirectionalLightData& aData)
//{
//	CGameObject* gameObject = new CGameObject(1);
//	gameObject->AddComponent<CEnviromentLightComponent>(*gameObject, aData.myColor, aData.myIntensity, aData.myDirection);
//	return std::move(gameObject);
//}

//CGameObject* CUnityFactory::CreateGameObject(const SPointLightData& aData)
//{
//	CGameObject* gameObject = new CGameObject(aData.myInstanceID);
//	gameObject->myTransform->Position(aData.myPosition);
//	gameObject->AddComponent<CPointLightComponent>(*gameObject, aData.myRange, aData.myColor, aData.myIntensity);
//	return std::move(gameObject);
//}

//CGameObject* CUnityFactory::CreateGameObject(const SGameObjectData& aData, const std::string& aModelPath)
//{
//	CGameObject* gameObject = new CGameObject(aData.myInstanceID);
//	gameObject->AddComponent<CModelComponent>(*gameObject, aModelPath);
//	gameObject->myTransform->Scale(aData.myScale);
//	gameObject->myTransform->Position(aData.myPosition);
//	gameObject->myTransform->Rotation(aData.myRotation);
//	return std::move(gameObject);
//}

//CGameObject* CUnityFactory::CreateGameObjectInstanced(const std::string& aModelPath, int InstancedID, std::vector<DirectX::SimpleMath::Matrix> aInstancedTransforms)
//{
//	CGameObject* gameObject = new CGameObject(InstancedID);
//	gameObject->AddComponent<CInstancedModelComponent>(*gameObject, aModelPath, aInstancedTransforms, (GetSuffixFromString(aModelPath) == "_AL"));
//	return std::move(gameObject);
//}
