#include "stdafx.h"
#include "RigidDynamicBody.h"
#include "Engine.h"
#include "PhysXWrapper.h"

std::string globalNames[] =
{
	"Biscuit",
	"Buttercup",
	"Cheeks",
	"Cookie",
	"Cupcake",
	"Daisy",
	"Doughnut",
	"Duncan",
	"Einstein",
	"Fluffy",
	"Hamilton",
	"Hamlet",
	"Hammy",
	"Nibbles",
	"Patches",
	"Whiskers"
};

CRigidDynamicBody::CRigidDynamicBody(physx::PxPhysics& aPhysX, const physx::PxTransform& aTransform)
{
	myBody = aPhysX.createRigidDynamic(aTransform);

	// Set userData to identify the collider (usually some GameObjectID)
	// It's used in ContactReportCallBack::onContact
	//myBody->userData = &globalNames[rand() % 16];

	// Locks Z since the demo only uses two axis
	//myBody->setRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z);

	// Tip! A material is not bound to a single body and can be reused
	// Use material as a seperate resource
	/*constexpr physx::PxReal staticFriction = 0.7f;
	constexpr physx::PxReal dynamicFriction = 0.9f;
	constexpr physx::PxReal restitution = 0.9f;*/
	//physx::PxMaterial* material = CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::basic);


	// Tip! A shape is not bound to a single body and can be reused
	// Use shape as a seperate resource
	/*constexpr float imageSize = 64;
	constexpr float fluffiness = 3;
	constexpr physx::PxReal radius = (imageSize / 10) - fluffiness;*/
	//physx::PxShape* shape = aPhysX.createShape(PxBoxGeometry(0.5f / 2.f, 0.6f / 2.f, 0.2f / 2.f), *CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::basic), true);
	//
	//myBody->attachShape(*shape);
}

Vector3 CRigidDynamicBody::GetPosition() const
{
	const physx::PxTransform transform = myBody->getGlobalPose();
	return { transform.p.x, transform.p.y , transform.p.z};
}

Quaternion CRigidDynamicBody::GetRotation() const
{
	const physx::PxTransform transform = myBody->getGlobalPose();
	return { transform.q.x,transform.q.y, transform.q.z, transform.q.w };
}
