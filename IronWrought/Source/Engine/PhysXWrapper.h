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


	enum ELayerMask
	{
		GROUP1 = (1 << 0),
		GROUP2 = (1 << 1),
		GROUP3 = (1 << 2),
		GROUP4 = (1 << 3)
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

	PxRaycastBuffer Raycast(Vector3 origin, Vector3 direction, float distance);
	PxMaterial* CreateMaterial(materialfriction amaterial);
	PxMaterial* CreateCustomMaterial(const float& aDynamicFriction, const float& aStaticFriction, const float& aBounciness);
	CRigidDynamicBody* CreateDynamicRigidbody(const CTransformComponent& aTransform);
	CRigidDynamicBody* CreateDynamicRigidbody(const PxTransform& aTransform);

	CCharacterController* CreateCharacterController(const Vector3& aPos, const float& aRadius, const float& aHeight, CTransformComponent* aUserData = nullptr, physx::PxUserControllerHitReport* aHitReport = nullptr);

	PxControllerManager* GetControllerManager();

	physx::PxUserControllerHitReport* GetPlayerReportBack() { return myPlayerReportCallback; }
	physx::PxUserControllerHitReport* GetEnemyReportBack() { return myEnemyReportCallback; }


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
	physx::PxUserControllerHitReport* myEnemyReportCallback;
	//std::queue<CRigidDynamicBody*> myAddBodyQueue;
};
