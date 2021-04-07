#include "stdafx.h"
#include "ConvexMeshColliderComponent.h"
#include "PhysXWrapper.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"
#include "Engine.h"
#include "Scene.h"

CConvexMeshColliderComponent::CConvexMeshColliderComponent(CGameObject& aParent)
	: CBehaviour(aParent)
	, myShape(nullptr)
{
}

CConvexMeshColliderComponent::~CConvexMeshColliderComponent()
{
}

void CConvexMeshColliderComponent::Awake()
{
	CRigidBodyComponent* rigidBody = nullptr;
	if (GameObject().TryGetComponent(&rigidBody))
	{
			rigidBody->GetDynamicRigidBody()->GetBody();
			//rigidBody->GetDynamicRigidBody()->GetBody().setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);
		if (myShape) {
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
}

void CConvexMeshColliderComponent::OnDisable()
{
}

void CConvexMeshColliderComponent::SetShape(physx::PxShape* aShape)
{
	myShape = aShape;
}
