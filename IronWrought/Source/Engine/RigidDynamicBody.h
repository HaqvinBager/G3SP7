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
	CRigidDynamicBody(physx::PxPhysics& aPhysX, const physx::PxTransform& aTransform);
	physx::PxRigidDynamic& GetBody() { return *myBody; }
	Vector3	GetPosition() const;
	Quaternion GetRotation() const;

	void SetPosition(const Vector3& aPos);
	void SetRotation(const Quaternion& aRot);

private:
	physx::PxRigidDynamic*	myBody = nullptr;
};
