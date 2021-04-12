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

	mySettings.myMaxDistance = 50.0f;
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
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
	{
		PostMaster::SCrossHairData data; // Wind down
		data.myIndex = 0;
		data.myShouldBeReversed = true;
		CMainSingleton::PostMaster().Send({ EMessageType::UpdateCrosshair, &data });

		Push();
	}
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Right))
	{
		Pull();
	}

	if (myCurrentTarget != nullptr)
	{
		Vector3 direction = -(myCurrentTarget->GameObject().myTransform->WorldPosition() - myGravitySlot->WorldPosition());
		float distance = direction.Length();
		float maxDistance = mySettings.myMaxDistance;

		mySettings.myCurrentDistanceInverseLerp = min(1.0f, InverseLerp(0.0f, maxDistance, distance));

		if (mySettings.myCurrentDistanceInverseLerp < 0.1f)
		{
			//myCurrentTarget->SetPosition(myGravitySlot->WorldPosition());
			//myCurrentTarget->SetRotation(myCurrentTarget->GetComponent<CTransformComponent>()->Rotation());
			myCurrentTarget->SetGlobalPose(myGravitySlot->WorldPosition(), myCurrentTarget->GetComponent<CTransformComponent>()->Rotation());
			myCurrentTarget->SetLinearVelocity({ 0.f, 0.f, 0.f });
			myCurrentTarget->SetAngularVelocity({ 0.f, 0.f, 0.f });
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

		PostMaster::SCrossHairData data; // Wind down
		data.myIndex = 0;
		data.myShouldBeReversed = true;
		CMainSingleton::PostMaster().Send({ EMessageType::UpdateCrosshair, &data });

		return;
	}

	Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
	Vector3 dir = -GameObject().myTransform->GetWorldMatrix().Forward();

	PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, mySettings.myMaxDistance);
//	std::vector<CGameObject*> gameobjects = CEngine::GetInstance()->GetActiveScene().ActiveGameObjects();

	/*for (int i = 0; i < gameobjects.size(); ++i) {
		if (gameobjects[i]->GetComponent<CRigidBodyComponent>()) {
			Vector3 pos = gameobjects[i]->myTransform->Position();
			pos -= start;
			pos.Normalize();
			dir.Normalize();
			float lookPrecentage = dir.Dot(pos);
			std::cout << i << ": " << lookPrecentage << std::endl;
			if (lookPrecentage > 0.99f && lookPrecentage > 0.f) {
				myCurrentTarget = gameobjects[i]->GetComponent<CRigidBodyComponent>();
			}
		}
	}*/

	if (hit.getNbAnyHits() > 0)
	{
		CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
		if (transform == nullptr)
		{
			PostMaster::SCrossHairData data; // Wind down
			data.myIndex = 0;
			data.myShouldBeReversed = true;
			CMainSingleton::PostMaster().Send({ EMessageType::UpdateCrosshair, &data });
			return;
		}

		CRigidBodyComponent* rigidbody = nullptr;
		if (transform->GameObject().TryGetComponent<CRigidBodyComponent>(&rigidbody))
		{
			if (!rigidbody->IsKinematic()) {
				myCurrentTarget = rigidbody;
				PostMaster::SCrossHairData data; // Wind Up
				data.myIndex = 0;
				CMainSingleton::PostMaster().Send({ EMessageType::UpdateCrosshair, &data });
			 }
		}

		//myCurrentTarget = transform->GetComponent<CRigidBodyComponent>();


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

#include "CameraComponent.h"
void CGravityGloveComponent::Push()
{
	if (myCurrentTarget != nullptr) {
		IRONWROUGHT->GetActiveScene().MainCamera()->SetTrauma(0.25f); // plz enable camera movement without moving player for shake??? ::)) Nico 2021-04-09

		myCurrentTarget->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(100.f);
		myCurrentTarget->AddForce(-GameObject().myTransform->GetWorldMatrix().Forward(), mySettings.myPushForce * myCurrentTarget->GetMass(), EForceMode::EImpulse);
		myCurrentTarget = nullptr;
	} else {
		Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
		Vector3 dir = -GameObject().myTransform->GetWorldMatrix().Forward();
		PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, mySettings.myMaxDistance);
		if (hit.getNbAnyHits() > 0)
		{
			CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
			if (transform == nullptr)
				return;

			CRigidBodyComponent* target = transform->GetComponent<CRigidBodyComponent>();
			if (target != nullptr) {
				if(!target->IsKinematic())
					target->AddForce(-GameObject().myTransform->GetWorldMatrix().Forward(), mySettings.myPushForce * target->GetMass(), EForceMode::EImpulse);
			}
		}
	}
}

void CGravityGloveComponent::OnEnable()
{
}

void CGravityGloveComponent::OnDisable()
{
}
