#pragma once
#include "Component.h"

namespace physx
{
	class PxShape;
	class PxRigidDynamic;
	class PxRigidStatic;
}

class CBoxColliderComponent : public CComponent
{
public:
	CBoxColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const Vector3& aBoxSize);
	~CBoxColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
private:
	physx::PxShape* myShape;
	physx::PxRigidDynamic* myDynamic;
	physx::PxRigidStatic* myStatic;
	Vector3 myPositionOffset;
	Vector3 myBoxSize;
};

