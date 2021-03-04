#pragma once
#include "pxPhysicsApi.h"
#include <utility>

namespace physx
{
	class PxController;
}

class CCharacterController
{
public:
	CCharacterController(const Vector3 aPosition);
	physx::PxController& GetController();

	Vector3 GetPosition() const;

private:
	physx::PxController* myController = nullptr;
};

