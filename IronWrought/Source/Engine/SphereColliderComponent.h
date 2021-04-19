#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
	class PxMaterial;
}

class CSphereColliderComponent : public CBehaviour
{
public:
	CSphereColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const float& aRadius, physx::PxMaterial* aMaterial = nullptr);
	~CSphereColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

private:
	physx::PxShape* myShape;
	physx::PxMaterial* myMaterial;
	Vector3 myPositionOffset;
	float myRadius;
};

