#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "PhysXWrapper.h"
#include "RigidBodyComponent.h"
#include "Engine.h"
#include "Scene.h"
#include "RigidDynamicBody.h"
#include "TransformComponent.h"

CBoxColliderComponent::CBoxColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const Vector3& aBoxSize) 
	: CBehaviour(aParent)
{
	//have check if it should have dynamic or static rigidbody
	myPositionOffset = aPositionOffset;
	myBoxSize = aBoxSize;
}

CBoxColliderComponent::~CBoxColliderComponent()
{
}

void CBoxColliderComponent::Awake()
{
}

void CBoxColliderComponent::Start()
{
	myShape = CEngine::GetInstance()->GetPhysx().GetPhysics()->createShape(physx::PxBoxGeometry(myBoxSize.x / 2.f, myBoxSize.y / 2.f, myBoxSize.z / 2.f), *CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::metal), true);
	myShape->setLocalPose({ myPositionOffset.x, myPositionOffset.y, myPositionOffset.z });
	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		myDynamic = &GameObject().GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetBody();
		GameObject().GetComponent<CRigidBodyComponent>()->AttachShape(myShape);
		myStatic = nullptr;
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
		myStatic = CEngine::GetInstance()->GetPhysx().GetPhysics()->createRigidStatic({ pos, pxQuat });
		myDynamic = nullptr;
	}
}

void CBoxColliderComponent::Update()
{
	if (Input::GetInstance()->IsKeyPressed('K'))
		myDynamic->addForce({ 0.0f, 10.0f, 0.0f }, physx::PxForceMode::eIMPULSE);
}

void CBoxColliderComponent::OnEnable()
{
}

void CBoxColliderComponent::OnDisable()
{
}
