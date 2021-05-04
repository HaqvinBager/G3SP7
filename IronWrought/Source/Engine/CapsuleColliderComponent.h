#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
	class PxMaterial;
}

class CCapsuleColliderComponent : public CBehaviour
{
public:
	CCapsuleColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const float& aRadius, const float& aHeight, physx::PxMaterial* aMaterial = nullptr);
	~CCapsuleColliderComponent() override;

	physx::PxShape* GetShape() { return myShape; }
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
	physx::PxMaterial* myMaterial;
};

