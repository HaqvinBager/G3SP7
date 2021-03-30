#include "stdafx.h"
#include "RigidBodyComponent.h"
#include "PhysXWrapper.h"
#include "RigidDynamicBody.h"
#include "Engine.h"
#include "Scene.h"


CRigidBodyComponent::CRigidBodyComponent(CGameObject& aParent)
	: CComponent(aParent)
{
}

CRigidBodyComponent::~CRigidBodyComponent()
{
	myDynamicRigidBody = nullptr;
}

void CRigidBodyComponent::Awake()
{
	
}

void CRigidBodyComponent::Start()
{
	DirectX::SimpleMath::Vector3 translation;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	DirectX::SimpleMath::Matrix transform = GameObject().myTransform->GetLocalMatrix();
	transform.Decompose(scale, quat, translation);
	PxVec3 pos = { translation.x, translation.y, translation.z };
	PxQuat pxQuat = { quat.x, quat.y, quat.z, quat.w };
	PxTransform pxTransform = { pos, pxQuat };
	myDynamicRigidBody = CEngine::GetInstance()->GetPhysx().CreateDynamicRigidbody(pxTransform);

	myDynamicRigidBody->GetBody().userData = (void*)GameObject().myTransform;
}

void CRigidBodyComponent::Update()
{
	if (myDynamicRigidBody != nullptr) {
		GameObject().myTransform->Position(myDynamicRigidBody->GetPosition());
	}
}

void CRigidBodyComponent::AddForce(Vector3 aDirection)
{
	myDynamicRigidBody->GetBody().addForce({aDirection.x, aDirection.y, aDirection.z});
}

void CRigidBodyComponent::AttachShape(physx::PxShape* aShape)
{
	bool status = myDynamicRigidBody->GetBody().attachShape(*aShape);
	if (status) {
		//CEngine::GetInstance()->GetActiveScene().PXScene()->addActor(myDynamicRigidBody->GetBody());
	}
}
