#pragma once
#include <PxPhysicsAPI.h>
#include <SimpleMath.h>

using namespace physx;

class ContactReportCallback;
class RigidDynamicBody;

class CPhysXWrapper
{
public:
	CPhysXWrapper();
	~CPhysXWrapper();

	bool Init();

	PxScene* CreatePXScene();

	void Simulate();

	RigidDynamicBody* CreateDynamicRigidbody(DirectX::SimpleMath::Vector3 aPos);
private:
	PxFoundation* myFoundation;
	PxPhysics* myPhysics;
	PxDefaultCpuDispatcher* myDispatcher;
	PxMaterial* myPXMaterial;
	PxPvd* myPhysicsVisualDebugger;
	PxDefaultAllocator* myAllocator;
	ContactReportCallback* myContactReportCallback;
};

