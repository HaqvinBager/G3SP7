#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
}

class CSphereColliderComponent : public CBehaviour
{
public:
	CSphereColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const float& aRadius, bool aIsStatic = false);
	~CSphereColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

private:
	physx::PxShape* myShape;
	Vector3 myPositionOffset;
	float myRadius;
};

