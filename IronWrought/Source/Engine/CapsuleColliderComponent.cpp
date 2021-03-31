#include "stdafx.h"
#include "CapsuleColliderComponent.h"
#include "PhysXWrapper.h"
#include "Engine.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"

CCapsuleColliderComponent::CCapsuleColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const float& aRadius, const float& aHeight)
	: CBehaviour(aParent)
	, myPositionOffset(aPositionOffset)
	, myRadius(aRadius)
	, myHeight(aHeight)
{

}

CCapsuleColliderComponent::~CCapsuleColliderComponent()
{

}

void CCapsuleColliderComponent::Awake()
{
}

void CCapsuleColliderComponent::Start()
{
	myShape = CEngine::GetInstance()->GetPhysx().GetPhysics()->createShape(physx::PxCapsuleGeometry(myRadius, myHeight * 0.5f), *CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::metal), true);
	PxVec3 offset = { myPositionOffset.x, myPositionOffset.y, myPositionOffset.z };
	PxTransform relativePose(offset, PxQuat(PxHalfPi, physx::PxVec3(0, 0, 1)));
	myShape->setLocalPose(relativePose);
	//myShape->setLocalPose(relativePose);

	//myShape->setLocalPose({ offset.x, offset.y, offset.z });
	if (GetComponent<CRigidBodyComponent>()) {
		myDynamic = &GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetBody();
		GetComponent<CRigidBodyComponent>()->AttachShape(myShape);
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

void CCapsuleColliderComponent::Update()
{
	int instanceID = 26686;
	if (Input::GetInstance()->IsKeyDown('K'))
	{
		if (GameObject().InstanceID() == instanceID)
		{
			myDynamic->addForce({ 5.1f , 0.0f, 0.0f }, physx::PxForceMode::eVELOCITY_CHANGE);
		}
	}
}

void CCapsuleColliderComponent::OnEnable()
{
}

void CCapsuleColliderComponent::OnDisable()
{
}
