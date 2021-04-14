#pragma once
#include <PxPhysicsAPI.h>

class CEnemyReportCallback : public physx::PxUserControllerHitReport
{
public:
	CEnemyReportCallback() = default;
	~CEnemyReportCallback() = default;

	void onShapeHit(const physx::PxControllerShapeHit& hit) override;
	void onControllerHit(const physx::PxControllersHit& hit) override;
	void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;
private:
};

