#include "stdafx.h"
#include "Scene.h"

#include <algorithm>

#include "Component.h"
#include "GameObject.h"
#include "TransformComponent.h"

#include "Model.h"
#include "ModelFactory.h"
#include "ModelComponent.h"
#include "InstancedModelComponent.h"

#include "SpriteInstance.h"
#include "AnimatedUIElement.h"
#include "TextInstance.h"

#include "LineInstance.h"
#include "LineFactory.h"

#include "EnvironmentLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "BoxLight.h"

#include "Engine.h"
#include "PostMaster.h"

#include "Camera.h"
#include "CameraComponent.h"

#include <PlayerControllerComponent.h>

#include "CollisionManager.h"

#include "NavmeshLoader.h"
#include "Canvas.h"

#include "Debug.h"
//SETUP START
CScene::CScene(const unsigned int aGameObjectCount)
	: myIsReadyToRender(false)
	, myMainCamera(nullptr)
	, myEnvironmentLight(nullptr)
	, myNavMesh(nullptr)
	, myNavMeshGrid(nullptr)
	, myPXScene(nullptr)
	, myPlayer(nullptr)
	, myCanvas(nullptr)
#ifdef _DEBUG
	, myGrid(nullptr)
#endif
{
	myGameObjects.reserve(aGameObjectCount);
	myPXScene = CEngine::GetInstance()->GetPhysx().CreatePXScene();

	myModelsToOutline.resize(2);
	for (unsigned int i = 0; i < myModelsToOutline.size(); ++i)
	{
		myModelsToOutline[i] = nullptr;
	}

#ifdef _DEBUG
	myShouldRenderLineInstance = false;
	//myGrid = new CLineInstance();
	//myGrid->Init(CLineFactory::GetInstance()->CreateGrid({ 0.1f, 0.5f, 1.0f, 1.0f }));
	//this->AddInstance(myGrid);
#endif
}

CScene::~CScene()
{
	this->ClearGameObjects();
	this->ClearSecondaryEnvironmentLights();
	this->ClearPointLights();
	this->ClearSpotLights();
	this->ClearBoxLights();

	myMainCamera = nullptr;
	delete myEnvironmentLight;
	myEnvironmentLight = nullptr;

	delete myCanvas;
	myCanvas = nullptr;

	myVFXTester = nullptr;
	myPlayer = nullptr;

	myIDGameObjectMap.clear();
	myComponentMap.clear();

#ifdef _DEBUG
	delete myGrid;
	myGrid = nullptr;
#endif
	if (myPXScene != nullptr)
	{
		myPXScene->release();
		myPXScene = nullptr;
	}

	// Any CScene that is not InGame's scene will not hold a NavMesh
	if (myNavMesh)
	{
		delete myNavMesh;
		myNavMesh = nullptr;
	}
	if (myNavMeshGrid)// -||-
	{
		delete myNavMeshGrid;
		myNavMeshGrid = nullptr;
	}
	// Even with this the memory still increases on every load!
}

bool CScene::Init()
{
	return true;
}

bool CScene::InitNavMesh(const std::string& aPath)
{
	CNavmeshLoader* loader = new CNavmeshLoader();
	myNavMesh = loader->LoadNavmesh(aPath);

	if (!myNavMesh)
	{
		return false;
	}

	std::vector<DirectX::SimpleMath::Vector3> positions;
	positions.resize(myNavMesh->myTriangles.size() * 6);

	for (UINT i = 0, j = 0; i < positions.size() && j < myNavMesh->myTriangles.size(); i += 6, j++)
	{
		positions[static_cast<int>(i + 0)] = myNavMesh->myTriangles[j]->myVertexPositions[0];
		positions[static_cast<int>(i + 1)] = myNavMesh->myTriangles[j]->myVertexPositions[1];
		positions[static_cast<int>(i + 2)] = myNavMesh->myTriangles[j]->myVertexPositions[2];
		positions[static_cast<int>(i + 3)] = myNavMesh->myTriangles[j]->myVertexPositions[0];
		positions[static_cast<int>(i + 4)] = myNavMesh->myTriangles[j]->myVertexPositions[1];
		positions[static_cast<int>(i + 5)] = myNavMesh->myTriangles[j]->myVertexPositions[2];
	}

	delete loader;
	loader = nullptr;
	return true;
}
bool CScene::InitCanvas(const std::string& aPath)
{
	if (!myCanvas)
		myCanvas = new CCanvas();

	myCanvas->Init(aPath);

	return true;
}
bool CScene::InitCanvas()
{
	if (!myCanvas)
		myCanvas = new CCanvas();

	myCanvas->Init();
	return true;
}
bool CScene::ReInitCanvas(const std::string& aPath, const bool& aDelete)
{
	if (aDelete)
	{
		delete myCanvas;
		myCanvas = nullptr;
	}

	return InitCanvas(aPath);;
}

void CScene::CanvasIsHUD(const bool& aIsHud)
{
	if (myCanvas)
		myCanvas->IsHUDCanvas(aIsHud);
}

void CScene::DisableWidgetsOnCanvas()
{
	if (myCanvas)
		myCanvas->DisableWidgets();
}

void CScene::CanvasToggle(const bool& anIsEnabled, const bool& anIsForceEnable)
{
	if (myCanvas)
	{
		if (anIsForceEnable)
			myCanvas->ForceEnabled(anIsEnabled);
		else
			myCanvas->SetEnabled(anIsEnabled);
	}
}

void CScene::Update()
{
	InitAnyNewComponents();

	for (auto& gameObject : myGameObjects)
		gameObject->Update();

	for (auto& gameObject : myGameObjects)
		gameObject->LateUpdate();

	if (myCanvas)
		myCanvas->Update();

	if (myPlayer && myEnvironmentLight)
	{
		myEnvironmentLight->SetPosition({ myPlayer->myTransform->WorldPosition().x, myPlayer->myTransform->WorldPosition().y + 20.0f, myPlayer->myTransform->WorldPosition().z });
	}

}

void CScene::FixedUpdate()
{
	for (auto& gameObject : myGameObjects)
		gameObject->FixedUpdate();
}

void CScene::InitAnyNewComponents()
{
	CallAwakeOnNewComponents();
	CallStartOnNewComponents();
}

void CScene::CallStartOnNewComponents()
{
	while (!myStartComponents.empty())
	{
		CComponent* component = myStartComponents.front();
		myStartComponents.pop();
		component->OnEnable();
		component->Start();
	}
}

void CScene::CallAwakeOnNewComponents()
{
	while (!myAwakeComponents.empty())
	{
		CComponent* component = myAwakeComponents.front();
		myAwakeComponents.pop();
		component->Awake();
		component->OnEnable();
		myStartComponents.push(component);
	}
}

void CScene::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case EMessageType::ComponentAdded:
		{
			CComponent* addedComponent = static_cast<CComponent*>(aMessage.data);
			myAwakeComponents.push(addedComponent);
		}
		break;
	}
}

//SETUP END
//SETTERS START
void CScene::MainCamera(CCameraComponent* aMainCamera)
{
	myMainCamera = aMainCamera;
}

void CScene::Player(CGameObject* aPlayerObject)
{
	myPlayer = aPlayerObject;
}

bool CScene::EnvironmentLight(CEnvironmentLight* anEnvironmentLight)
{
	myEnvironmentLight = anEnvironmentLight;
	return true;
}

void CScene::ShouldRenderLineInstance(const bool aShouldRender)
{
#ifdef  _DEBUG
	myShouldRenderLineInstance = aShouldRender;
#else
	aShouldRender;
#endif //  _DEBUG
}
//SETTERS END
//GETTERS START
CCameraComponent* CScene::MainCamera()
{
	return myMainCamera;
}

CGameObject* CScene::Player()
{
	return myPlayer;
}

CPlayerControllerComponent* CScene::PlayerController()
{
	if (myPlayer)
		return myPlayer->GetComponent<CPlayerControllerComponent>();
	return nullptr;
}

CEnvironmentLight* CScene::EnvironmentLight()
{
	return myEnvironmentLight;
}

SNavMesh* CScene::NavMesh()
{
	return myNavMesh;
}

PxScene* CScene::PXScene()
{
	return myPXScene;
}

std::vector<CGameObject*> CScene::ModelsToOutline() const
{
	return myModelsToOutline;
}

std::vector<CPointLight*>& CScene::PointLights()
{
	return myPointLights;
}

CCanvas* CScene::Canvas()
{
	return myCanvas;
}

const std::vector<CGameObject*>& CScene::ActiveGameObjects() const
{
	return myGameObjects;
}
std::vector<CEnvironmentLight*> CScene::CullSecondaryEnvironmentLights(CGameObject* /*aGameObject*/)
{
	return mySecondaryEnvironmentLights;
}
//GETTERS END
//CULLING START
std::vector<CPointLight*> CScene::CullPointLights(CGameObject* /*aGameObject*/)
{
	//std::cout << __FUNCTION__ << " Reminde to add actual culling to this function!" << std::endl;
	return myPointLights;
}

std::vector<CSpotLight*> CScene::CullSpotLights(CGameObject* /*aGameObject*/)
{
	return mySpotLights;
}

std::vector<CBoxLight*> CScene::CullBoxLights(CGameObject* /*aGameObject*/)
{
	return myBoxLights;
}


std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> CScene::CullLights(CGameObject* aGameObject)
{
	std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> pointLightPair;
	UINT counter = 0;
	for (UINT i = 0; i < myPointLights.size(); ++i)
	{
		float distanceSquared = DirectX::SimpleMath::Vector3::DistanceSquared(myPointLights[i]->GetPosition(), aGameObject->GetComponent<CTransformComponent>()->Transform().Translation());
		float range = myPointLights[i]->GetRange();
		if (distanceSquared < (range * range))
		{
			pointLightPair.second[counter] = myPointLights[i];
			++counter;

			if (counter == LIGHTCOUNT)
			{
				break;
			}
		}
	}

	pointLightPair.first = counter;
	return pointLightPair;
}

const std::vector<CLineInstance*>& CScene::CullLineInstances() const
{
#ifdef _DEBUG
	return myLineInstances;
#else
	return myLineInstances;
#endif
}

const std::vector<SLineTime>& CScene::CullLines() const
{
	return CDebug::GetInstance()->GetLinesTime();
	//return CDebug::GetInstance()->GetLines();
}

LightPair CScene::CullLightInstanced(CInstancedModelComponent* aModelType)
{
	//S�tt s� att Range t�cker objektet l�ngst bort
	//if (myPlayer != nullptr) {
	//	aModelType->GameObject().myTransform->Position(GetPlayer()->myTransform->Position());
	//}

	std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> pointLightPair;
	UINT counter = 0;
	for (UINT i = 0; i < myPointLights.size(); ++i)
	{
		float distanceSquared = DirectX::SimpleMath::Vector3::DistanceSquared(myPointLights[i]->GetPosition(), aModelType->GetComponent<CTransformComponent>()->Position());
		float range = myPointLights[i]->GetRange() * 200.0f;
		if (distanceSquared < (range * range))
		{
			pointLightPair.second[counter] = myPointLights[i];
			++counter;

			if (counter == LIGHTCOUNT)
			{
				break;
			}
		}
	}

	pointLightPair.first = counter;
	return pointLightPair;
}

std::vector<CGameObject*> CScene::CullGameObjects(CCameraComponent* aMainCamera)
{
	auto& viewFrustum = aMainCamera->GetViewFrustum();
	DirectX::BoundingSphere currentSphere;
	std::vector<CGameObject*> culledGameObjects;
	for (auto& gameObject : myGameObjects)
	{
		if (gameObject->InstanceID() == PLAYER_CAMERA_ID)
		{
			culledGameObjects.push_back(gameObject);
			continue;
		}

		if (gameObject->GetComponent<CInstancedModelComponent>())
		{
			culledGameObjects.push_back(gameObject);
			continue;
		}

		currentSphere = DirectX::BoundingSphere(gameObject->myTransform->Position(), 24.0f);
		if (viewFrustum.Intersects(currentSphere))
		{
			culledGameObjects.push_back(gameObject);
		}
	}
	return culledGameObjects;

	//return myGameObjects;
}

CGameObject* CScene::FindObjectWithID(const int aGameObjectInstanceID)
{
	if (myIDGameObjectMap.find(aGameObjectInstanceID) == myIDGameObjectMap.end())
		return nullptr;

	return myIDGameObjectMap[aGameObjectInstanceID];
}
bool CScene::AddInstance(CEnvironmentLight* aSecondaryDirectionalLight)
{
	mySecondaryEnvironmentLights.emplace_back(aSecondaryDirectionalLight);
	return true;
}
//CULLING END
//POPULATE SCENE START
bool CScene::AddInstance(CPointLight* aPointLight)
{
	myPointLights.emplace_back(aPointLight);
	return true;
}

bool CScene::AddInstance(CSpotLight* aSpotLight)
{
	mySpotLights.emplace_back(aSpotLight);
	return true;
}

bool CScene::AddInstance(CBoxLight* aBoxLight)
{
	myBoxLights.emplace_back(aBoxLight);
	return true;
}

bool CScene::AddInstance(CLineInstance* aLineInstance)
{
	myLineInstances.emplace_back(aLineInstance);
	return true;
}

bool CScene::AddInstance(CGameObject* aGameObject)
{
	//Lägg in dom i en "Next frame i will be initied vector, Then when they are inited we move it into myGameObjects

	myGameObjects.emplace_back(aGameObject);
	myIDGameObjectMap[aGameObject->InstanceID()] = aGameObject;

	for (auto& component : aGameObject->myComponents) {
		myComponentMap[typeid(*component).hash_code()].push_back(component.get());
	}

	return true;
}

bool CScene::AddInstances(std::vector<CGameObject*>& someGameObjects)
{
	if (someGameObjects.size() == 0)
		return false;

	for (unsigned int i = 0; i < someGameObjects.size(); ++i)
	{
		AddInstance(someGameObjects[i]);
		//myGameObjects.emplace_back(someGameObjects[i]);
	}


	//myGameObjects.insert(myGameObjects.end(), someGameObjects.begin(), someGameObjects.end());
	return true;
}

//PhysX
bool CScene::AddPXScene(PxScene* aPXScene)
{
	if (!aPXScene)
	{
		return false;
	}
	myPXScene = aPXScene;
	return true;
}
//POPULATE SCENE END
// 
//REMOVE SPECIFIC INSTANCE START
bool CScene::RemoveInstance(CEnvironmentLight* aSecondaryEnvironmentLight)
{
	for (int i = 0; i < mySecondaryEnvironmentLights.size(); ++i)
	{
		if (aSecondaryEnvironmentLight == mySecondaryEnvironmentLights[i])
		{
			mySecondaryEnvironmentLights.erase(mySecondaryEnvironmentLights.begin() + i);
			return true;
		}
	}
	return false;
}
bool CScene::RemoveInstance(CPointLight* aPointLight)
{
	for (int i = 0; i < myPointLights.size(); ++i)
	{
		if (aPointLight == myPointLights[i])
		{
			//std::swap(myGameObjects[i], myGameObjects[myGameObjects.size() - 1]);
			//myGameObjects.pop_back();
			myPointLights.erase(myPointLights.begin() + i);
			return true;
		}
	}
	return false;
}

bool CScene::RemoveInstance(CSpotLight* aSpotLight)
{
	for (int i = 0; i < mySpotLights.size(); ++i)
	{
		if (aSpotLight == mySpotLights[i])
		{
			mySpotLights.erase(mySpotLights.begin() + i);
			return true;
		}
	}
	return false;
}

bool CScene::RemoveInstance(CBoxLight* aBoxLight)
{
	for (int i = 0; i < myBoxLights.size(); ++i)
	{
		if (aBoxLight == myBoxLights[i])
		{
			myBoxLights.erase(myBoxLights.begin() + i);
			return true;
		}
	}
	return false;
}

bool CScene::RemoveInstance(CGameObject* aGameObject)
{
	for (int i = 0; i < myGameObjects.size(); ++i)
	{
		if (aGameObject == myGameObjects[i])
		{
			//std::swap(myGameObjects[i], myGameObjects[myGameObjects.size() - 1]);
			//myGameObjects.pop_back();
			myGameObjects.erase(myGameObjects.begin() + i);
			return true;
		}
	}
	return false;
}

bool CScene::ClearSecondaryEnvironmentLights()
{
	for (auto& p : mySecondaryEnvironmentLights)
	{
		delete p;
		p = nullptr;
	}
	mySecondaryEnvironmentLights.clear();
	return true;
}
//REMOVE SPECIFIC INSTANCE END
// 
//CLEAR SCENE OF INSTANCES START
bool CScene::ClearPointLights()
{
	for (auto& p : myPointLights)
	{
		delete p;
		p = nullptr;
	}
	myPointLights.clear();
	return true;
}

bool CScene::ClearSpotLights()
{
	for (auto& p : mySpotLights)
	{
		delete p;
		p = nullptr;
	}
	mySpotLights.clear();
	return true;
}

bool CScene::ClearBoxLights()
{
	for (auto& p : myBoxLights)
	{
		delete p;
		p = nullptr;
	}
	myBoxLights.clear();
	return true;
}

bool CScene::ClearLineInstances()
{
	for (size_t i = 0; i < myLineInstances.size(); ++i)
	{
		if (myLineInstances[i] != nullptr)
		{
			delete myLineInstances[i];
			myLineInstances[i] = nullptr;
		}
	}
	return false;
}

bool CScene::ClearGameObjects()
{
	for (size_t i = 0; i < myGameObjects.size(); ++i)
	{
		delete myGameObjects[i];
		myGameObjects[i] = nullptr;
	}
	myGameObjects.clear();
	return true;
}

//CLEAR SCENE OF INSTANCES END

bool CScene::NearestPlayerComparer::operator()(const CPointLight* a, const CPointLight* b) const
{
	float dist0 = Vector3::DistanceSquared(a->GetPosition(), myPos);
	float dist1 = Vector3::DistanceSquared(b->GetPosition(), myPos);
	return dist0 < dist1;
}