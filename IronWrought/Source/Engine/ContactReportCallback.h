#include <pxPhysicsApi.h>

namespace physx
{
	class PxActor;
	struct PxTriggerPair;
	struct PxContactPairHeader;
	struct PxContactPair;
	struct PxConstraintInfo;
}

class CContactReportCallback : public physx::PxSimulationEventCallback
{
public:
						CContactReportCallback() = default;
						~CContactReportCallback() = default;
	void				onWake(physx::PxActor** actors, physx::PxU32 count) override;
	void				onSleep(physx::PxActor** actors, physx::PxU32 count) override;
	void				onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
	void				onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;
	void				onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;
	void				onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;

private:
};
