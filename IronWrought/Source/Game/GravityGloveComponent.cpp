#include "stdafx.h"
#include "GravityGloveComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "PhysXWrapper.h"
#include "LineFactory.h"
#include "LineInstance.h"
#include "Engine.h"
#include "Scene.h"
#include "RigidDynamicBody.h"

CGravityGloveComponent::CGravityGloveComponent(CGameObject& aParent, CTransformComponent* aGravitySlot)
	: CBehaviour(aParent)
	, myGravitySlot(aGravitySlot)
{
	mySettings.myPushForce = 27.f;
	//mySettings.myDistanceToMaxLinearVelocity = 2.5f;
	mySettings.myMaxPushForce = 100.0f;
	mySettings.myMinPushForce = 10.0f;
	mySettings.myMinPullForce = 1.5f;

	mySettings.myMaxDistance = 10.0f;
	mySettings.myCurrentDistanceInverseLerp = 0.0f;
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
		PostMaster::SCrossHairData data;
		data.myIndex = 0;
		data.myShouldBeReversed = true;
		CMainSingleton::PostMaster().Send({ EMessageType::UpdateCrosshair, &data });
		Push();
	}
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Right)) {
		PostMaster::SCrossHairData data;
		data.myIndex = 0;
		CMainSingleton::PostMaster().Send({ EMessageType::UpdateCrosshair, &data });
		Pull();
	}

	// Crosshair Animations:
	//	Shoot = 0 , Spin-open and Spin-close (Spin-open => reversed = true)
	//	Idle = 1
	//	Hold = 2
	//	Spin-close : Idle -> Hold
	//	Spin-open  : Hold -> Idle

	if (myCurrentTarget != nullptr)
	{
		Vector3 direction = -(myCurrentTarget->GameObject().myTransform->WorldPosition() - myGravitySlot->WorldPosition());
		float distance = direction.Length();
		float maxDistance = mySettings.myMaxDistance;

		mySettings.myCurrentDistanceInverseLerp = min(1.0f, InverseLerp(0.0f, maxDistance, distance));

		if (mySettings.myCurrentDistanceInverseLerp < 0.1f)
		{
			myCurrentTarget->SetPosition(myGravitySlot->WorldPosition());
			myCurrentTarget->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(1.0f);
		}
		else
		{
			float force = Lerp(mySettings.myMaxPushForce, mySettings.myMinPullForce, mySettings.myCurrentDistanceInverseLerp);

			//myCurrentTarget->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(max(mySettings.myDistanceToMaxLinearVelocity, distance));
			direction.Normalize();
			myCurrentTarget->AddForce(direction, force, EForceMode::EForce);
		}
		//Yaay Here things are happening omfg lets gouee! : D
	}
}

void CGravityGloveComponent::Pull()
{
	if (myCurrentTarget != nullptr)
	{
		myCurrentTarget->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(100.f);
		myCurrentTarget = nullptr;
		return;
	}

	Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
	Vector3 dir = -GameObject().myTransform->GetWorldMatrix().Forward();

	PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, mySettings.myMaxDistance);
	if (hit.getNbAnyHits() > 0)
	{
		CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
		if (transform == nullptr)
			return;
		
		myCurrentTarget = transform->GetComponent<CRigidBodyComponent>();

	#ifdef _DEBUG
		CLineInstance* myLine = new CLineInstance();
		CLineInstance* myLine2 = new CLineInstance();
		myLine->Init(CLineFactory::GetInstance()->CreateLine(start, { hit.getAnyHit(0).position.x, hit.getAnyHit(0).position.y, hit.getAnyHit(0).position.z }, { 0,255,0,255 }));
		myLine2->Init(CLineFactory::GetInstance()->CreateLine(start, start + (dir * 5.f), { 255,0,0,255 }));
		CEngine::GetInstance()->GetActiveScene().AddInstance(myLine);
		CEngine::GetInstance()->GetActiveScene().AddInstance(myLine2);
	#endif
	}
	//myCurrentTarget->SetPosition(myGravitySlot->WorldPosition());
}

void CGravityGloveComponent::Push()
{
	if (myCurrentTarget != nullptr) {
		PostMaster::SCrossHairData data;
		data.myIndex = 0;
		data.myShouldBeReversed = true;
		CMainSingleton::PostMaster().Send({ EMessageType::UpdateCrosshair, &data });

		myCurrentTarget->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(100.f);
		myCurrentTarget->AddForce(-GameObject().myTransform->GetWorldMatrix().Forward(), mySettings.myPushForce * myCurrentTarget->GetMass(), EForceMode::EImpulse);
		myCurrentTarget = nullptr;
	}
}

void CGravityGloveComponent::OnEnable()
{
}

void CGravityGloveComponent::OnDisable()
{
}
