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
