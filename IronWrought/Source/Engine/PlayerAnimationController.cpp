#include "stdafx.h"
#include "PlayerAnimationController.h"

#define UINT_CAST(a) static_cast<unsigned int>(a)

CPlayerAnimationController::CPlayerAnimationController()
{}

CPlayerAnimationController::~CPlayerAnimationController()
{}

void CPlayerAnimationController::Init(CAnimationComponent* anAnimationComponent)
{
	myPlayerAnimation = anAnimationComponent;
	myPlayerAnimation->BlendLerpBetween(UINT_CAST(EPlayerAnimations::Idle), UINT_CAST(EPlayerAnimations::Walk), 0.0f);
}

void CPlayerAnimationController::Update()
{
	myReturnToIdle -= CTimer::Dt();
	if (myReturnToIdle <= 0.0f)
		Idle();
}

void CPlayerAnimationController::UpdateBlendValue(const float aBlendValue)
{
	myPlayerAnimation->BlendLerp(aBlendValue);
}

void CPlayerAnimationController::SwitchBlendTarget(const EPlayerAnimations anAnimationToBlendTo, const EPlayerAnimations anAnimationToBlendFrom)
{
	myPlayerAnimation->BlendLerpBetween(UINT_CAST(anAnimationToBlendFrom), UINT_CAST(anAnimationToBlendTo), 0.0f);
}

void CPlayerAnimationController::Idle()
{
	myPlayerAnimation->BlendToAnimation(UINT_CAST(EPlayerAnimations::Idle), 1.0f);

}
void CPlayerAnimationController::Walk()
{
	myPlayerAnimation->BlendToAnimation(UINT_CAST(EPlayerAnimations::Walk), 0.3f);
	myReturnToIdle = 1.0f;
}
void CPlayerAnimationController::Pull(const float anObjectDistanceToPlayer, const float anInitialDistance)
{
	float blend = anObjectDistanceToPlayer / anInitialDistance;
	myPlayerAnimation->BlendLerpBetween(UINT_CAST(EPlayerAnimations::Pull), UINT_CAST(EPlayerAnimations::PullHover), blend);
	myReturnToIdle = 1.0f;
}
void CPlayerAnimationController::Push()
{
	myPlayerAnimation->BlendToAnimation(UINT_CAST(EPlayerAnimations::Push), 0.2f, true, true);
	myReturnToIdle = 1.0f;
}
void CPlayerAnimationController::TakeDamage()
{
	myPlayerAnimation->BlendToAnimation(UINT_CAST(EPlayerAnimations::TakingDamage), 0.2f, true, true);
}