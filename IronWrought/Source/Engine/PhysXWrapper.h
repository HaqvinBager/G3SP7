#pragma once
#include <PxPhysicsAPI.h>
#include <queue>

using namespace physx;

class CContactReportCallback;
class CRigidDynamicBody;
class CScene;
class CCharacterController;
class CDynamicRigidBody;
class CCharacterReportCallback;

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


	enum ELayerMask : physx::PxU32
	{
		STATIC_ENVIRONMENT = (1 << 0),
		DYNAMIC_OBJECTS = (1 << 1),
		WORLD = STATIC_ENVIRONMENT | DYNAMIC_OBJECTS,	

		PLAYER = (1 << 8),
		ENEMY = (1 << 9),

		//Om man lägger tilll ELayerMask Layers vill man addera dom nedan så att "all" alltid representerar alla layers //Axel Savage 2021-05-07
		ALL = STATIC_ENVIRONMENT | DYNAMIC_OBJECTS | PLAYER | ENEMY /* | NyttLayer  */ //Axel Savage 2021-05-07
	};

public:

	CPhysXWrapper();
	~CPhysXWrapper();

	bool Init();
	void Simulate();

	PxScene* CreatePXScene(CScene* aScene);
	PxScene* GetPXScene();
	PxPhysics* GetPhysics() { return myPhysics; }


	bool TryRayCast(const Vector3& aOrigin, Vector3& aDirection, const float aDistance, PxRaycastBuffer& outHit);

	PxRaycastBuffer Raycast(Vector3 origin, Vector3 direction, float distance, ELayerMask aLayerMask = ELayerMask::ALL);
	PxRaycastBuffer Raycast(Vector3 origin, Vector3 direction, float distance, physx::PxU32 aLayerMask);

	PxMaterial* CreateMaterial(materialfriction amaterial);
	PxMaterial* CreateCustomMaterial(const float& aDynamicFriction, const float& aStaticFriction, const float& aBounciness);
	CRigidDynamicBody* CreateDynamicRigidbody(const CTransformComponent& aTransform);
	CRigidDynamicBody* CreateDynamicRigidbody(const PxTransform& aTransform);

	CCharacterController* CreateCharacterController(const Vector3& aPos, const float& aRadius, const float& aHeight, CTransformComponent* aUserData = nullptr, physx::PxUserControllerHitReport* aHitReport = nullptr);

	PxControllerManager* GetControllerManager();

	physx::PxUserControllerHitReport* GetPlayerReportBack() { return myPlayerReportCallback; }


  //merge conflict 8/3/2021
	//void DebugLines();
	void Cooking(const std::vector<CGameObject*>& gameObjectsToCook, CScene* aScene);
	physx::PxShape* CookObject(CGameObject& aGameObject);

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
	//std::unordered_map<PxScene*, PxControllerManager*> myControllerManagers;// Should not be necessary
	std::unordered_map<CScene*, PxScene*> myPXScenes;
	physx::PxUserControllerHitReport* myPlayerReportCallback;
	//std::queue<CRigidDynamicBody*> myAddBodyQueue;
};
