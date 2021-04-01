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

void CRigidBodyComponent::AddForce(const Vector3& aDirection)
{
	myDynamicRigidBody->GetBody().addForce({aDirection.x, aDirection.y, aDirection.z}, PxForceMode::eIMPULSE);
}

void CRigidBodyComponent::AddForce(const physx::PxVec3& aDirection)
{
	myDynamicRigidBody->GetBody().addForce(aDirection, PxForceMode::eIMPULSE);
}

void CRigidBodyComponent::AttachShape(physx::PxShape* aShape)
{
	bool status = myDynamicRigidBody->GetBody().attachShape(*aShape);
	if (status) {
		//CEngine::GetInstance()->GetActiveScene().PXScene()->addActor(myDynamicRigidBody->GetBody());
	}
}
