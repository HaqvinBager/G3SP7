#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
	class PxMaterial;
}

class CConvexMeshColliderComponent : public CBehaviour
{
public:
	CConvexMeshColliderComponent(CGameObject& aParent, physx::PxMaterial* aMaterial = nullptr);
	~CConvexMeshColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	void SetShape(physx::PxShape* aShape);
	const physx::PxShape* GetShape() { return myShape; }

	const physx::PxMaterial* GetMaterial() { return myMaterial; }

private:
	physx::PxShape* myShape;
	physx::PxMaterial* myMaterial;
};

