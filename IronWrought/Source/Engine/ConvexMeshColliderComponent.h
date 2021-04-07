#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
}

class CConvexMeshColliderComponent : public CBehaviour
{
public:
	CConvexMeshColliderComponent(CGameObject& aParent);
	~CConvexMeshColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void SetShape(physx::PxShape* aShape);
	const physx::PxShape* GetShape() { return myShape; }

private:
	physx::PxShape* myShape;
};

