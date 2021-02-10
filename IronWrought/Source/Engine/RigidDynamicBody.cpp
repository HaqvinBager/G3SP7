#include "stdafx.h"
#include "RigidDynamicBody.h"

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

RigidDynamicBody::RigidDynamicBody(physx::PxPhysics& aPhysX, const Vector3 aPosition)
{
	myBody = aPhysX.createRigidDynamic({ aPosition.x, aPosition.y, aPosition.z });

	// Set userData to identify the collider (usually some GameObjectID)
	// It's used in ContactReportCallBack::onContact
	myBody->userData = &globalNames[rand() % 16];

	// Locks Z since the demo only uses two axis
	myBody->setRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z);

	// Tip! A material is not bound to a single body and can be reused
	// Use material as a seperate resource
	constexpr physx::PxReal staticFriction = 0.7f;
	constexpr physx::PxReal dynamicFriction = 0.9f;
	constexpr physx::PxReal restitution = 0.9f;
	physx::PxMaterial* material = aPhysX.createMaterial(staticFriction, dynamicFriction, restitution);

	// Tip! A shape is not bound to a single body and can be reused
	// Use shape as a seperate resource
	constexpr float imageSize = 64;
	constexpr float fluffiness = 3;
	constexpr physx::PxReal radius = (imageSize) - fluffiness;
	physx::PxShape* shape = aPhysX.createShape(physx::PxSphereGeometry(radius), *material, true);

	myBody->attachShape(*shape);
}

Vector3 RigidDynamicBody::GetPosition() const
{
	const physx::PxTransform transform = myBody->getGlobalPose();
	return { transform.p.x, transform.p.y , transform.p.z};
}

float RigidDynamicBody::GetRotation() const
{
	return myBody->getGlobalPose().q.z;
}
