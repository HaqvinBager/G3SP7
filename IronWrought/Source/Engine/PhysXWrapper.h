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

	enum class materialfriction
	{
		metal,
		wood,
		bounce,
		basic,
		none


	};

public:

	CPhysXWrapper();
	~CPhysXWrapper();

	bool Init();

	PxScene* CreatePXScene(CScene* aScene);
	PxScene* GetPXScene();
	PxPhysics* GetPhysics() { return myPhysics; }


	PxRaycastBuffer Raycast(Vector3 origin, Vector3 direction, float distance);
	void RaycastHit(PxRaycastBuffer aHit);


	PxMaterial* CreateMaterial(materialfriction amaterial);

	void Simulate();

	CRigidDynamicBody* CreateDynamicRigidbody(const PxTransform& aTransform);

	CCharacterController* CreateCharacterController(const Vector3& aPos, const float& aRadius, const float& aHeight);

	PxControllerManager* GetControllerManager();


  //merge conflict 8/3/2021
	//void DebugLines();
	void Cooking(std::vector<CGameObject*> gameObjectsToCook, CScene* aScene);

private:


	PxFoundation* myFoundation;
	PxPhysics* myPhysics;
	PxDefaultCpuDispatcher* myDispatcher;
	PxMaterial* myPXMaterial;
	PxPvd* myPhysicsVisualDebugger;
	PxDefaultAllocator* myAllocator;
	PxCooking* myCooking;
	CContactReportCallback* myContactReportCallback;
	PxControllerManager* myControllerManager;
	std::unordered_map<PxScene*, PxControllerManager*> myControllerManagers;// Should not be necessary
	std::unordered_map<CScene*, PxScene*> myPXScenes;
};
