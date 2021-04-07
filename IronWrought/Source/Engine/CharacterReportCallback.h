#pragma once
#include <PxPhysicsAPI.h>

class CCharacterReportCallback : public physx::PxUserControllerHitReport
{
public:
	CCharacterReportCallback() = default;
	~CCharacterReportCallback() = default;

	void onShapeHit(const physx::PxControllerShapeHit &hit) override;
	void onControllerHit(const physx::PxControllersHit& hit) override;
	void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;
private:
};

