#include "stdafx.h"
#include "SphereColliderComponent.h"
#include "PhysXWrapper.h"
#include "Engine.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"

CSphereColliderComponent::CSphereColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const float& aRadius) 
	: CBehaviour(aParent)
{
	myPositionOffset = aPositionOffset;
	myRadius = aRadius;
}

CSphereColliderComponent::~CSphereColliderComponent()
{
}

void CSphereColliderComponent::Awake()
{
}

void CSphereColliderComponent::Start()
{
	myShape = CEngine::GetInstance()->GetPhysx().GetPhysics()->createShape(physx::PxSphereGeometry(myRadius), *CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::metal), true);
	myShape->setLocalPose({ myPositionOffset.x, myPositionOffset.y, myPositionOffset.z });
	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		myDynamic = &GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetBody();
		GameObject().GetComponent<CRigidBodyComponent>()->AttachShape(myShape);
		myStatic = nullptr;
	}
	else {
		DirectX::SimpleMath::Vector3 translation;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion quat;
		DirectX::SimpleMath::Matrix transform = GameObject().GetComponent<CTransformComponent>()->GetLocalMatrix();
		transform.Decompose(scale, quat, translation);

		PxVec3 pos = { translation.x, translation.y, translation.z };
		PxQuat pxQuat = { quat.x, quat.y, quat.z, quat.w };
		myStatic = CEngine::GetInstance()->GetPhysx().GetPhysics()->createRigidStatic({ pos, pxQuat });
		myDynamic = nullptr;
	}
}

void CSphereColliderComponent::Update()
{
}

void CSphereColliderComponent::OnEnable()
{
}

void CSphereColliderComponent::OnDisable()
{
}
