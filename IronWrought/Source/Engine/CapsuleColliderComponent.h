#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
}

class CCapsuleColliderComponent : public CBehaviour
{
public:
	CCapsuleColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const float& aRadius, const float& aHeight);
	~CCapsuleColliderComponent() override;

public:
	void Awake() override;
	void Start() override;
	void Update()override;

public:
	void OnEnable() override;
	void OnDisable() override;

private:
	float myRadius;
	float myHeight;
	Vector3 myPositionOffset;
	physx::PxShape* myShape;
};

