#include "stdafx.h"
#include "CapsuleColliderComponent.h"
#include "PhysXWrapper.h"
#include "Engine.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"

CCapsuleColliderComponent::CCapsuleColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const float& aRadius, const float& aHeight, PxMaterial* aMaterial)
	: CBehaviour(aParent)
	, myPositionOffset(aPositionOffset)
	, myRadius(aRadius)
	, myHeight(aHeight)
	, myMaterial(aMaterial)
	, myShape(nullptr)
{
	if (myMaterial == nullptr) {
		myMaterial = CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::basic);
	}
}

CCapsuleColliderComponent::~CCapsuleColliderComponent()
{

}

void CCapsuleColliderComponent::Awake()
{
}

void CCapsuleColliderComponent::Start()
{
	myShape = CEngine::GetInstance()->GetPhysx().GetPhysics()->createShape(physx::PxCapsuleGeometry(myRadius, myHeight * 0.5f), *myMaterial, true);
	PxVec3 offset = { myPositionOffset.x, myPositionOffset.y, myPositionOffset.z };
	PxTransform relativePose(offset, PxQuat(PxHalfPi, physx::PxVec3(0, 0, 1)));
	myShape->setLocalPose(relativePose);

	CRigidBodyComponent* rigidBody = nullptr;
	if (GameObject().TryGetComponent(&rigidBody))
	{
		rigidBody->GetDynamicRigidBody()->GetBody();
		rigidBody->AttachShape(myShape);
	}
	else
	{
		DirectX::SimpleMath::Vector3 translation;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion quat;
		DirectX::SimpleMath::Matrix transform = GameObject().GetComponent<CTransformComponent>()->GetLocalMatrix();
		transform.Decompose(scale, quat, translation);

		PxVec3 pos = { translation.x, translation.y, translation.z };
		PxQuat pxQuat = { quat.x, quat.y, quat.z, quat.w };
		CEngine::GetInstance()->GetPhysx().GetPhysics()->createRigidStatic({ pos, pxQuat });
	}
}

void CCapsuleColliderComponent::Update()
{
}

void CCapsuleColliderComponent::OnEnable()
{
}

void CCapsuleColliderComponent::OnDisable()
{
}
