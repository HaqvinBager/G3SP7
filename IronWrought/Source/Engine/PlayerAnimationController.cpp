#include "stdafx.h"
#include "PlayerAnimationController.h"

#include "MainSingleton.h"
#include "PostMaster.h"

#define UINT_CAST(a) static_cast<unsigned int>(a)

inline const float ClampF(const float& aValue, const float& aMin, const float& aMax)
{
	return (aValue > aMin ? (aValue < aMax ? aValue : 1.0f) : 0.0f);
}

void CPlayerAnimationController::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::GravityGlovePull:
		{
			Pull();
		}break;

		case EMessageType::GravityGlovePush:
		{
			Push();
		}break;


		case EMessageType::GravityGloveTargetDistance:
		{
			if (myState != EState::Action)
				return;

			PostMaster::SGravityGloveTargetData targetData = *static_cast<PostMaster::SGravityGloveTargetData*>(aMessage.data);
			float percent = fabs(targetData.myCurrentDistanceSquared - targetData.myInitialDistanceSquared) / targetData.myInitialDistanceSquared;
			myPlayerAnimation->BlendLerp(percent);

		}break;

		default:break;
	}
}

CPlayerAnimationController::CPlayerAnimationController()
	: myPlayerAnimation(nullptr)
	, myState(EState::Idle)
{}

CPlayerAnimationController::~CPlayerAnimationController()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::GravityGlovePull, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::GravityGlovePush, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::GravityGloveTargetDistance, this);
}

void CPlayerAnimationController::Init(CAnimationComponent* anAnimationComponent)
{
	myPlayerAnimation = anAnimationComponent;
	myPlayerAnimation->BlendLerpBetween(UINT_CAST(EPlayerAnimations::Idle), UINT_CAST(EPlayerAnimations::Walk), 0.0f);

	CMainSingleton::PostMaster().Subscribe(EMessageType::GravityGlovePull, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::GravityGlovePush, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::GravityGloveTargetDistance, this);
}

void CPlayerAnimationController::Update(const Vector3& aMovement)
{
	if (myPlayerAnimation->PlayingTemporaryAnimation())
		return;

	Walk(aMovement);
}

void CPlayerAnimationController::Walk(const Vector3& aMovement)
{
	if (myState == EState::Action)
		return;

	if ((fabs(aMovement.x) + fabs(aMovement.z)) > 0.0f)
	{
		myState = EState::Moving;
		myPlayerAnimation->BlendToAnimation(UINT_CAST(EPlayerAnimations::Walk), 0.3f);
	}
	else
	{
		myState = EState::Idle;
		myPlayerAnimation->BlendToAnimation(UINT_CAST(EPlayerAnimations::Idle), 1.0f);
	}

	// Walk & Idle animations seem to have different durations, looks wonky.
		//float xPlusZ = (fabs(aMovement.x) + fabs(aMovement.z));
		//float movement = ClampF( xPlusZ + (0.5f * xPlusZ), 0.f, 1.0f);
		//myPlayerAnimation->BlendLerpBetween(UINT_CAST(EPlayerAnimations::Idle), UINT_CAST(EPlayerAnimations::Walk), movement);
}

void CPlayerAnimationController::Pull()
{
	myState = myState == EState::Action ? EState::Transition : EState::Action;
	myPlayerAnimation->BlendLerpBetween(UINT_CAST(EPlayerAnimations::Pull), UINT_CAST(EPlayerAnimations::PullHover), 0.0f);
}

void CPlayerAnimationController::Push()
{
	myPlayerAnimation->BlendLerpBetween(UINT_CAST(EPlayerAnimations::Idle), UINT_CAST(EPlayerAnimations::Walk), 0.0f);// Make sure it returns to desired animation :/
	myPlayerAnimation->BlendToAnimation(UINT_CAST(EPlayerAnimations::Push), 0.15f, true, true);
	myState = EState::Transition;
}

void CPlayerAnimationController::TakeDamage()
{
	myPlayerAnimation->BlendLerpBetween(UINT_CAST(EPlayerAnimations::Idle), UINT_CAST(EPlayerAnimations::Walk), 0.0f);// Make sure it returns to desired animation :/
	myPlayerAnimation->BlendToAnimation(UINT_CAST(EPlayerAnimations::TakingDamage), 0.2f, true, true);
	myState = EState::Transition;
}
