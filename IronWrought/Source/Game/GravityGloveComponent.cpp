#include "stdafx.h"
#include "GravityGloveComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "PhysXWrapper.h"
#include "LineFactory.h"
#include "LineInstance.h"
#include "Engine.h"
#include "Scene.h"

CGravityGloveComponent::CGravityGloveComponent(CGameObject& aParent, CTransformComponent* aGravitySlot)
	: CBehaviour(aParent)
	, myGravitySlot(aGravitySlot)
{
}

CGravityGloveComponent::~CGravityGloveComponent()
{
	myGravitySlot = nullptr;
}

void CGravityGloveComponent::Awake()
{
}

void CGravityGloveComponent::Start()
{
}

void CGravityGloveComponent::Update()
{
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left)) {
		Pull();
	}
}

void CGravityGloveComponent::Pull()
{
	Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
	Vector3 dir = -GameObject().myTransform->GetWorldMatrix().Forward();


	PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, 100000);

	CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
	CRigidBodyComponent* rigidBody = transform->GetComponent<CRigidBodyComponent>();
	rigidBody->SetPosition(myGravitySlot->Position());
	
#ifdef _DEBUG
	CLineInstance* myLine = new CLineInstance();
	CLineInstance* myLine2 = new CLineInstance();
	myLine->Init(CLineFactory::GetInstance()->CreateLine(start, { hit.getAnyHit(0).position.x, hit.getAnyHit(0).position.y, hit.getAnyHit(0).position.z }, { 0,255,0,255 }));
	myLine2->Init(CLineFactory::GetInstance()->CreateLine(start, start + (dir * 5.f), { 255,0,0,255 }));
	CEngine::GetInstance()->GetActiveScene().AddInstance(myLine);
	CEngine::GetInstance()->GetActiveScene().AddInstance(myLine2);
#endif
}

void CGravityGloveComponent::Push()
{
}

void CGravityGloveComponent::OnEnable()
{
}

void CGravityGloveComponent::OnDisable()
{
}
