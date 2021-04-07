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
		if (myShape) {
			rigidBody->AttachShape(myShape);
			rigidBody->GetDynamicRigidBody()->GetBody().setCMassLocalPose({ -5.149889e-07f, 0.5043119f,  1.865765e-05f});
			rigidBody->GetDynamicRigidBody()->GetBody().setMass(3.f);
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
