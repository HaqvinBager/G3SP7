#include "stdafx.h"
#include "NodeTypeEnemySpawn.h"

#include "NodeInstance.h"

#include "GameObject.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "Scene.h"

#include <thread>
#include <mutex>

CNodeTypeEnemySpawn::CNodeTypeEnemySpawn()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));//0
	myPins.push_back(SPin("Nr of enemies: ", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EInt));//1
	DeclareDataOnPinIfNecessary<int>(myPins[1], 0);

	//DeclareDataOnPinIfNecessary<int>(myPins[1], 1.0f);
}

int CNodeTypeEnemySpawn::OnEnter(CNodeInstance * aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	int nrToSpawn = NodeData::Get<int>(someData);

	const std::vector<CGameObject*> gameObjects = aTriggeringNodeInstance->GetCurrentGameObject();
	std::thread spawnThread(SpawnEnemies, nrToSpawn, gameObjects[1]->myTransform->Position()/*i, std::ref(*this), std::ref(loadSuccessful)*/);
	spawnThread.detach();
	//SpawnEnemies(nrToSpawn, gameObjects[1]->myTransform->Position());

	return -1;
}

std::mutex gNodeTypeEnemySpawn_Mutex;
void CNodeTypeEnemySpawn::SpawnEnemies(const int aNumber, const Vector3& aSpawnPosition)
{
	gNodeTypeEnemySpawn_Mutex.lock();
	for (size_t i = 0; i < aNumber; ++i)
	{
		CGameObject* go = new CGameObject(-99999 + (int)i);
		go->AddComponent<CModelComponent>(*go, ASSETPATH("Assets/Graphics/Character/Enemy/CH_E_Robot_SK.fbx"));
		go->myTransform->Position(aSpawnPosition);
		go->Awake();
		go->Start();
		IRONWROUGHT_ACTIVE_SCENE.AddInstance(go);
	}
	gNodeTypeEnemySpawn_Mutex.unlock();
}
