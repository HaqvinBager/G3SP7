#pragma once
#include "pxPhysicsApi.h"
#include <utility>
	
namespace physx
{
	class PxRigidDynamic;
	class PxPhysics;
}

class RigidDynamicBody
{
public:
							RigidDynamicBody(physx::PxPhysics& aPhysX, const Vector3 aPosition);
	physx::PxRigidDynamic&	GetBody();
	Vector3	GetPosition() const;
	float					GetRotation() const;

private:
	physx::PxRigidDynamic*	myBody = nullptr;
};

inline physx::PxRigidDynamic& RigidDynamicBody::GetBody()
{
	return *myBody;
}

