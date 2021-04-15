#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "PhysXWrapper.h"
#include "RigidBodyComponent.h"
#include "Engine.h"
#include "Scene.h"
#include "RigidDynamicBody.h"
#include "TransformComponent.h"

CBoxColliderComponent::CBoxColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const Vector3& aBoxSize, const bool aIsTrigger, PxMaterial* aMaterial)
	: CBehaviour(aParent)
	, myShape(nullptr)
	, myPositionOffset(aPositionOffset)
	, myBoxSize(aBoxSize)
	, myMaterial(aMaterial)
	, myIsTrigger(aIsTrigger)
{
	if (myMaterial == nullptr) {
		myMaterial = CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::basic);
	}
}

CBoxColliderComponent::~CBoxColliderComponent()
{
}

void CBoxColliderComponent::Awake()
{
	CreateBoxCollider();
}

void CBoxColliderComponent::Start()
{
}

void CBoxColliderComponent::Update()
{
}

void CBoxColliderComponent::CreateBoxCollider()
{
	myShape = CEngine::GetInstance()->GetPhysx().GetPhysics()->createShape(physx::PxBoxGeometry(myBoxSize.x / 2.f, myBoxSize.y / 2.f, myBoxSize.z / 2.f), *myMaterial, true);
	myShape->setLocalPose({ myPositionOffset.x, myPositionOffset.y, myPositionOffset.z });
	PxFilterData filterData;
	filterData.word0 =	CPhysXWrapper::ELayerMask::GROUP1;
	myShape->setQueryFilterData(filterData);
	//myShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

	CRigidBodyComponent* rigidBody = nullptr;
	if (GameObject().TryGetComponent(&rigidBody))
	{
		rigidBody->AttachShape(myShape);
		rigidBody->GetDynamicRigidBody()->GetBody();

		//dynamic.setMaxLinearVelocity(10.f);
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
		PxRigidStatic* actor = CEngine::GetInstance()->GetPhysx().GetPhysics()->createRigidStatic({ pos, pxQuat });
		actor->attachShape(*myShape);
		CEngine::GetInstance()->GetPhysx().GetPXScene()->addActor(*actor);
		if (myIsTrigger) {
			PxShape* triggerShape;
			actor->getShapes(&triggerShape, 1);
			triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			actor->userData = (void*) this;
		}
	}
}

void CBoxColliderComponent::OnTriggerEnter()
{
	bool state = true;
	SStringMessage message = { myEventMessage.c_str(), &state };
	CMainSingleton::PostMaster().Send(message);
}

void CBoxColliderComponent::OnTriggerExit()
{
	bool state = false;
	SStringMessage message = { myEventMessage.c_str(), &state };
	CMainSingleton::PostMaster().Send(message);
}

void CBoxColliderComponent::OnEnable()
{
}

void CBoxColliderComponent::OnDisable()
{
}
