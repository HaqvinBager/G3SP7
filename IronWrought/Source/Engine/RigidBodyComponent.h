#pragma once
#include "Component.h"

class CRigidDynamicBody;
class CScene;
class CPhysXWrapper;

namespace physx
{
	class PxTransform;
	class PxShape;
	class PxScene;
	class PxVec3;
}

enum class EForceMode
{
	EForce,				//!< parameter has unit of mass * distance/ time^2, i.e. a force
	EImpulse,			//!< parameter has unit of mass * distance /time
	EVelocityChange,	//!< parameter has unit of distance / time, i.e. the effect is mass independent: a velocity change.
	EAcceleration		//!< parameter has unit of distance/ time^2, i.e. an acceleration. It gets treated just like a force except the mass is not divided out before integration.
};

class CRigidBodyComponent : public CComponent
{
public:
	CRigidBodyComponent(CGameObject& aParent);

	~CRigidBodyComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void AddForce(const Vector3& aDirection, const EForceMode aForceMode = EForceMode::EForce);
	void AddForce(const Vector3& aDirection, const float aForce, const EForceMode aForceMode = EForceMode::EForce);
	void SetPosition(const Vector3& aPos);

	void AttachShape(physx::PxShape* aShape);

	const float GetMass();
	
	CRigidDynamicBody* GetDynamicRigidBody() { return myDynamicRigidBody; }
private:
	void AddForce(const physx::PxVec3& aDirection, const EForceMode aForceMode);

private:
	CRigidDynamicBody* myDynamicRigidBody;
};