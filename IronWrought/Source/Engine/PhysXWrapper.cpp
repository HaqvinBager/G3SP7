#include "PhysXWrapper.h"
#include "ContactReportCallback.h"
#include "Timer.h"
#include "Scene.h"
#include "Engine.h"
#include "RigidDynamicBody.h"

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eCONTACT_DEFAULT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_CONTACT_POINTS
		| PxPairFlag::eDETECT_CCD_CONTACT;

	return PxFilterFlag::eDEFAULT;
}

CPhysXWrapper::CPhysXWrapper()
{
	myFoundation = nullptr;
	myPhysics = nullptr;
	myDispatcher = nullptr;
	myPXMaterial = nullptr;
	myPhysicsVisualDebugger = nullptr;
	myAllocator = nullptr;
	myContactReportCallback = nullptr;
}

CPhysXWrapper::~CPhysXWrapper()
{
	//I will fix later -- crashes because cant release nullptr //Alexander Matthäi 15/1 - 2021

	//myPXMaterial->release();
	//myDispatcher->release();
	//myPhysics->release();
	//myPhysicsVisualDebugger->release();
	//myFoundation->release();
	//delete myAllocator;
	//myAllocator = nullptr;
	myPhysicsVisualDebugger->disconnect();
}

bool CPhysXWrapper::Init()
{
	PxDefaultErrorCallback errorCallback;

	myAllocator = new PxDefaultAllocator();
	myFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myAllocator, errorCallback);
	if (!myFoundation) {
		return false;
	}
	myPhysicsVisualDebugger = PxCreatePvd(*myFoundation);
	if (!myPhysicsVisualDebugger) {
		return false;
	}
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
	//PxPvdTransport* transport = PxDefaultPvdFileTransportCreate("Test.pxd2");
	myPhysicsVisualDebugger->connect(*transport, PxPvdInstrumentationFlag::eALL);

	myPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *myFoundation, PxTolerancesScale(), true, myPhysicsVisualDebugger);
	if (!myPhysics) {
		return false;
	}

	// All collisions gets pushed to this class
	myContactReportCallback = new ContactReportCallback();
    return true;
}

PxScene* CPhysXWrapper::CreatePXScene()
{
	PxSceneDesc sceneDesc(myPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.82f, 0.0f);
	myDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = myDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = myContactReportCallback;
	PxScene* pXScene = myPhysics->createScene(sceneDesc);
	if (!pXScene) {
		return nullptr;
	}

	PxPvdSceneClient* pvdClient = pXScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	// Create a basic setup for a scene - contain the rodents in a invisible cage
	PxMaterial* myMaterial = myPhysics->createMaterial(1.0f, 0.0f, -0.5f);

	PxRigidStatic* groundPlane = PxCreatePlane(*myPhysics, PxPlane(0, 1, 0, 3.3f), *myMaterial);
	//groundPlane->setGlobalPose( {15.0f,0.0f,0.0f} );
	pXScene->addActor(*groundPlane);

	return pXScene;
}



PxMaterial* CPhysXWrapper::CreateMaterial(materialfriction amaterial)
{
	switch ((materialfriction)amaterial)
	{
	case materialfriction::metal:
		return myPhysics->createMaterial(1.0f, 1.0f, 0.0f);
		break;
	case materialfriction::wood:
		return myPhysics->createMaterial(0.2f, 0.5f, 0.3f);

		break;
	case materialfriction::bounce:
		return myPhysics->createMaterial(0.0f, 0.0f, 1.0f);
		break;
	case materialfriction::none:
		return myPXMaterial;
		break;
	default:
		break;
	}
	return nullptr;
}

void CPhysXWrapper::Simulate()
{
	if (CEngine::GetInstance()->GetActiveScene().PXScene() != nullptr) {
		CEngine::GetInstance()->GetActiveScene().PXScene()->simulate(CTimer::Dt());
		CEngine::GetInstance()->GetActiveScene().PXScene()->fetchResults(true);
	}
}

RigidDynamicBody* CPhysXWrapper::CreateDynamicRigidbody(Vector3 aPos)
{
	RigidDynamicBody* dynamicBody = new RigidDynamicBody(*myPhysics, aPos);
	CEngine::GetInstance()->GetActiveScene().PXScene()->addActor(dynamicBody->GetBody());
	return dynamicBody;
}
