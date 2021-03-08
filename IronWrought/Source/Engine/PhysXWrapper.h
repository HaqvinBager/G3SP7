#pragma once
#include <PxPhysicsAPI.h>

using namespace physx;

class CContactReportCallback;
class CRigidDynamicBody;
class CScene;
class CCharacterController;

class CPhysXWrapper
{
public:
	CPhysXWrapper();
	~CPhysXWrapper();

	bool Init();

	physx::PxScene* CreatePXScene(CScene* aScene);
	PxScene* GetPXScene();
	PxPhysics* GetPhysics() { return myPhysics; }

	void Simulate();

	CRigidDynamicBody* CreateDynamicRigidbody(const Vector3& aPos);

	CCharacterController* CreateCharacterController(PxControllerShapeType::Enum aType, const Vector3& aPos, const float& aRadius, const float& aHeight);

	PxControllerManager* GetControllerManger();
private:
	PxFoundation* myFoundation;
	PxPhysics* myPhysics;
	PxDefaultCpuDispatcher* myDispatcher;
	PxMaterial* myPXMaterial;
	PxPvd* myPhysicsVisualDebugger;
	PxDefaultAllocator* myAllocator;
	CContactReportCallback* myContactReportCallback;
	//PxControllerManager* myControllerManager;
	std::unordered_map<PxScene*, PxControllerManager*> myControllerManagers;
	std::unordered_map<CScene*, PxScene*> myPXScenes;
};

