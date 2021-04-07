#include "stdafx.h"
#include "RigidBodyComponent.h"
#include "PhysXWrapper.h"
#include "RigidDynamicBody.h"
#include "Engine.h"
#include "Scene.h"

//CRigidBodyComponent::CRigidBodyComponent(CGameObject& aParent)
//	: CComponent(aParent)
//	, myDynamicRigidBody(nullptr)
//{
//}

CRigidBodyComponent::CRigidBodyComponent(CGameObject& aParent)
	: CComponent(aParent)
	, myDynamicRigidBody(nullptr)
{
}

CRigidBodyComponent::~CRigidBodyComponent()
{
	myDynamicRigidBody = nullptr;
}

void CRigidBodyComponent::Awake()
{
	myDynamicRigidBody = CEngine::GetInstance()->GetPhysx().CreateDynamicRigidbody(*GameObject().myTransform);
	myDynamicRigidBody->GetBody().userData = (void*)GameObject().myTransform;
}

void CRigidBodyComponent::Start()
{

}

void CRigidBodyComponent::Update()
{
	if (myDynamicRigidBody != nullptr) {
		GameObject().myTransform->Rotation(myDynamicRigidBody->GetRotation());
		GameObject().myTransform->Position(myDynamicRigidBody->GetPosition());
	}
}

void CRigidBodyComponent::AddForce(const Vector3& aDirection, const EForceMode aForceMode)
{
	AddForce(PxVec3(aDirection.x, aDirection.y, aDirection.z), aForceMode);
}

void CRigidBodyComponent::AddForce(const Vector3& aNormalizedDirection, const float aForce, const EForceMode aForceMode)
{
	Vector3 aDirectionWithForce = aNormalizedDirection * aForce;
	AddForce(PxVec3(aDirectionWithForce.x, aDirectionWithForce.y, aDirectionWithForce.z), aForceMode);
}

void CRigidBodyComponent::AddAngularForce(const Vector3& aAngularForce, const EForceMode aForceMode)
{
	PxVec3 angularForce = PxVec3(aAngularForce.x, aAngularForce.y, aAngularForce.z);
	myDynamicRigidBody->GetBody().addTorque(angularForce, (PxForceMode::Enum)aForceMode);

}

void CRigidBodyComponent::SetAngularVelocity(const Vector3& aVelocity)
{
	myDynamicRigidBody->GetBody().setAngularVelocity({ aVelocity.x, aVelocity.y, aVelocity.z });
}

void CRigidBodyComponent::AddForce(const physx::PxVec3& aDirectionAndForce, const EForceMode aForceMode)
{
	myDynamicRigidBody->GetBody().addForce(aDirectionAndForce, (PxForceMode::Enum)aForceMode);
}

void CRigidBodyComponent::AttachShape(physx::PxShape* aShape)
{
	bool status = myDynamicRigidBody->GetBody().attachShape(*aShape);
	if (status) {
		//CEngine::GetInstance()->GetActiveScene().PXScene()->addActor(myDynamicRigidBody->GetBody());
	}
}

const float CRigidBodyComponent::GetMass()
{
	PxReal mass = myDynamicRigidBody->GetBody().getMass();
	return {mass};
}

void CRigidBodyComponent::SetPosition(const Vector3& aPos) {
	myDynamicRigidBody->GetBody().setGlobalPose({aPos.x, aPos.y, aPos.z});
}
