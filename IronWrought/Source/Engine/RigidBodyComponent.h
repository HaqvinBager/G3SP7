#pragma once
#include "Component.h"

class CRigidDynamicBody;
class CScene;

namespace physx
{
	class PxTransform;
	class PxShape;
	class PxScene;
}


class CRigidBodyComponent : public CComponent
{
public:
	CRigidBodyComponent(CGameObject& aParent);
	~CRigidBodyComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void AddForce(Vector3 aDirection);

	void AttachShape(physx::PxShape* aShape);

	CRigidDynamicBody* GetDynamicRigidBody() { return myDynamicRigidBody; }
private:
	CRigidDynamicBody* myDynamicRigidBody;
};

