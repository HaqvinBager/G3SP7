#pragma once
#include <PxPhysicsAPI.h>
using namespace physx;

class CCharacterController
{
public:
public:
	CCharacterController(PxControllerShapeType::Enum aType, const Vector3& aPos, const float& aRadius, const float& aHeight);
	~CCharacterController();

	bool InitCapsuelController(const Vector3& aPos, const float& aRadius, const float& aHeight);
	bool InitBoxController(const Vector3& aPos, const float& aRadius, const float& aHeight);

	PxController& GetController() { return *myController; };

	Vector3 GetPosition() const;
private:
	PxController* myController;
};
