#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
	class PxRigidDynamic;
	class PxRigidStatic;
}

class CSphereColliderComponent : public CBehaviour
{
public:
	CSphereColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const float& aRadius);
	~CSphereColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

private:
	physx::PxShape* myShape;
	physx::PxRigidDynamic* myDynamic;
	physx::PxRigidStatic* myStatic;
	Vector3 myPositionOffset;
	float myRadius;
};

