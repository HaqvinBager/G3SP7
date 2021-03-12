#pragma once
#include "pxPhysicsApi.h"
#include <utility>
	
namespace physx
{
	class PxRigidDynamic;
	class PxPhysics;
}

class CRigidDynamicBody
{
public:
							CRigidDynamicBody(physx::PxPhysics& aPhysX, int aInstanceID, const Vector3& aPosition);
	physx::PxRigidDynamic&	GetBody();
	Vector3	GetPosition() const;
	float					GetRotation() const;

private:
	physx::PxRigidDynamic*	myBody = nullptr;
};

inline physx::PxRigidDynamic& CRigidDynamicBody::GetBody()
{
	return *myBody;
}

