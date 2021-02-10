#pragma once
#include <PxPhysicsAPI.h>

using namespace physx;

class ContactReportCallback;
class RigidDynamicBody;

enum class materialfriction 
{
	metal,
	wood,
	bounce,
	none

};

class CPhysXWrapper
{
public:
	CPhysXWrapper();
	~CPhysXWrapper();

	bool Init();

	PxScene* CreatePXScene();


	PxMaterial* CreateMaterial(materialfriction amaterial);

	void Simulate();

	RigidDynamicBody* CreateDynamicRigidbody(Vector3 aPos);
private:
	PxFoundation* myFoundation;
	PxPhysics* myPhysics;
	PxDefaultCpuDispatcher* myDispatcher;
	PxMaterial* myPXMaterial;
	PxPvd* myPhysicsVisualDebugger;
	PxDefaultAllocator* myAllocator;
	ContactReportCallback* myContactReportCallback;
};

