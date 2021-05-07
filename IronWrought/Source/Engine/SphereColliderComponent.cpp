#include "stdafx.h"
#include "SphereColliderComponent.h"
#include "PhysXWrapper.h"
#include "Engine.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"

CSphereColliderComponent::CSphereColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const float& aRadius, PxMaterial* aMaterial) 
	: CBehaviour(aParent)
	, myShape(nullptr)
	, myMaterial(aMaterial)
{
	myPositionOffset = aPositionOffset;
	myRadius = aRadius;
	if (myMaterial == nullptr) {
		myMaterial = CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::basic);
	}
}

CSphereColliderComponent::~CSphereColliderComponent()
{
}

void CSphereColliderComponent::Awake()
{
}

void CSphereColliderComponent::Start()
{
	myShape = CEngine::GetInstance()->GetPhysx().GetPhysics()->createShape(physx::PxSphereGeometry(myRadius), *myMaterial, true);
	myShape->setLocalPose({ myPositionOffset.x, myPositionOffset.y, myPositionOffset.z });
	myShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	PxFilterData filterData;
	filterData.word0 = CPhysXWrapper::ELayerMask::DYNAMIC_OBJECTS;
	myShape->setQueryFilterData(filterData);
	CRigidBodyComponent* rigidBody = nullptr;
	if (GameObject().TryGetComponent(&rigidBody))
	{
		rigidBody->GetDynamicRigidBody()->GetBody();
		rigidBody->AttachShape(myShape);
	}
	else {
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

void CSphereColliderComponent::Update()
{
}

void CSphereColliderComponent::OnEnable()
{
	Enabled(true);
}

void CSphereColliderComponent::OnDisable()
{
	Enabled(false);
}
