#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
}

class CBoxColliderComponent : public CBehaviour
{
public:
	CBoxColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const Vector3& aBoxSize, bool aIsStatic = false);
	~CBoxColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;
private:
	physx::PxShape* myShape;
	Vector3 myPositionOffset;
	Vector3 myBoxSize;
	bool created;
};

