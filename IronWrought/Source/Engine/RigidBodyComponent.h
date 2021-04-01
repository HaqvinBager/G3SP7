#pragma once
#include "Component.h"

class CRigidDynamicBody;
class CScene;
class CPhysXWrapper;

namespace physx
{
	class PxTransform;
	class PxShape;
	class PxScene;
	class PxVec3;
}


class CRigidBodyComponent : public CComponent
{
public:
	CRigidBodyComponent(CGameObject& aParent);

	~CRigidBodyComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void AddForce(const Vector3& aDirection);
	void AddForce(const Vector3& aDirection, const float aForce);
	void SetPosition(const Vector3& aPos);

	void AttachShape(physx::PxShape* aShape);
	
	CRigidDynamicBody* GetDynamicRigidBody() { return myDynamicRigidBody; }
private:
	void AddForce(const physx::PxVec3& aDirection);

private:
	CRigidDynamicBody* myDynamicRigidBody;
};