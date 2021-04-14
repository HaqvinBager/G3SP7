#pragma once
#include "pxPhysicsApi.h"
#include <utility>

namespace physx
{
    class PxController;
    class PxUserControllerHitReport;
}

class CPlayerReportCallback;

class CTransformComponent;

class CCharacterController
{
public:
    CCharacterController(const Vector3 aPosition, const float aRadius = 0.6f, const float aHeight = 1.8f, CTransformComponent* userData = nullptr, physx::PxUserControllerHitReport* aHitReport = nullptr);
    physx::PxController& GetController();

    Vector3 GetPosition() const;
    void SetPosition(const Vector3& aPosition);
    UINT8 Move(const Vector3& aDir, float aSpeed);
    const Vector3& GetHitNormal();

private:
    physx::PxController* myController = nullptr;
    CPlayerReportCallback* myPlayerReport;
};

//#pragma once
//#include <PxPhysicsAPI.h>
//using namespace physx;
//
//class CCharacterController
//{
//public:
//public:
//	CCharacterController(PxControllerShapeType::Enum aType, const Vector3& aPos, const float& aRadius, const float& aHeight);
//	~CCharacterController();
//
//	bool InitCapsuelController(const Vector3& aPos, const float& aRadius, const float& aHeight);
//	bool InitBoxController(const Vector3& aPos, const float& aRadius, const float& aHeight);
//
//	PxController& GetController() { return *myController; };
//
//	Vector3 GetPosition() const;
//private:
//	PxController* myController;
//};
