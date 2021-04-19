#pragma once
#include <PxPhysicsAPI.h>

class CPlayerReportCallback : public physx::PxUserControllerHitReport
{
public:
	CPlayerReportCallback() = default;
	~CPlayerReportCallback() = default;

	void onShapeHit(const physx::PxControllerShapeHit &hit) override;
	void onControllerHit(const physx::PxControllersHit& hit) override;
	void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;

	const Vector3 GetNormal() const;
	
private:
	physx::PxVec3 myHitNormal;
};

