#pragma once
#include <PxPhysicsAPI.h>

using namespace physx;

class ContactReportCallback;
class RigidDynamicBody;
class CGameObject;
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

	PxScene* CreatePXScene();


	PxRaycastBuffer Raycast(Vector3 origin, Vector3 direction, float distance);
	void RaycastHit(PxVec3 position, PxVec3 normal);
	

	PxMaterial* CreateMaterial(materialfriction amaterial);

	void Simulate();

	RigidDynamicBody* CreateDynamicRigidbody(Vector3 aPos);

	CCharacterController* CreateCharacterController(Vector3 aPos = {0.f, 0.f, 0.f});

	PxControllerManager& GetControllerManager() { return *myControllerManager; }

	void DebugLines();

	void Cooking(std::vector<CGameObject*> gameObjectsToCook, CScene* aScene);
private:
	
	
	PxFoundation* myFoundation;
	PxPhysics* myPhysics;
	PxDefaultCpuDispatcher* myDispatcher;
	PxMaterial* myPXMaterial;
	PxPvd* myPhysicsVisualDebugger;
	PxDefaultAllocator* myAllocator;
	PxCooking* myCooking;
	ContactReportCallback* myContactReportCallback;
	PxControllerManager* myControllerManager;
};

