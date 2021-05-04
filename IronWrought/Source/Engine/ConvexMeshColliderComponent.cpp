#include "stdafx.h"
#include "ConvexMeshColliderComponent.h"
#include "PhysXWrapper.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"
#include "Engine.h"
#include "Scene.h"

CConvexMeshColliderComponent::CConvexMeshColliderComponent(CGameObject& aParent, PxMaterial* aMaterial)
	: CBehaviour(aParent)
	, myShape(nullptr)
	, myMaterial(aMaterial)
{
	if (myMaterial == nullptr) {
		myMaterial = CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::basic);
	}
}

CConvexMeshColliderComponent::~CConvexMeshColliderComponent()
{
	myShape = nullptr;
}

void CConvexMeshColliderComponent::Awake()
{
	CRigidBodyComponent* rigidBody = nullptr;
	if (GameObject().TryGetComponent(&rigidBody))
	{
			rigidBody->GetDynamicRigidBody()->GetBody();
			//rigidBody->GetDynamicRigidBody()->GetBody().setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);
		if (myShape) {
			PxFilterData filterData;
			filterData.word0 = CPhysXWrapper::ELayerMask::GROUP1;
			myShape->setQueryFilterData(filterData);
			rigidBody->AttachShape(myShape);
		}
	}
}

void CConvexMeshColliderComponent::Start()
{
}

void CConvexMeshColliderComponent::Update()
{
}

void CConvexMeshColliderComponent::OnEnable()
{
	Enabled(true);
}

void CConvexMeshColliderComponent::OnDisable()
{
	Enabled(false);
}

void CConvexMeshColliderComponent::SetShape(physx::PxShape* aShape)
{
	myShape = aShape;
}
