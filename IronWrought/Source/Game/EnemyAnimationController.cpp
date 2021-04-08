#include "stdafx.h"
#include "EnemyAnimationController.h"

#include "AnimationComponent.h"

#define UINT_CAST(a) static_cast<unsigned int>(a)

CEnemyAnimationController::CEnemyAnimationController()
 : myAnimationComponent(nullptr)
	, myIsActive(true)
{}

CEnemyAnimationController::~CEnemyAnimationController()
{
	myAnimationComponent = nullptr;
}

void CEnemyAnimationController::Init(CAnimationComponent* anAnimationComponent)
{
	myAnimationComponent = anAnimationComponent;
	myAnimationComponent->BlendLerpBetween(UINT_CAST(EEnemyAnimations::Idle), UINT_CAST(EEnemyAnimations::Search), 0.0f);
	myIsActive = true;
}

void CEnemyAnimationController::Update()
{
	myReturnToIdle -= CTimer::Dt();
	if (myReturnToIdle <= 0.0f)
		Idle();
}

void CEnemyAnimationController::UpdateBlendValue(const float aBlendValue)
{
	if (!myIsActive)
		return;
	myAnimationComponent->BlendLerp(aBlendValue);
}

void CEnemyAnimationController::SwitchBlendTarget(const EEnemyAnimations anAnimationToBlendTo, const EEnemyAnimations anAnimationToBlendFrom)
{
	if (!myIsActive)
		return;
	myAnimationComponent->BlendLerpBetween(UINT_CAST(anAnimationToBlendFrom), UINT_CAST(anAnimationToBlendTo), 0.0f);
}

void CEnemyAnimationController::Idle()
{
	if (!myIsActive)
		return;
	myAnimationComponent->BlendToAnimation(UINT_CAST(EEnemyAnimations::Idle), 1.0f);
}

void CEnemyAnimationController::Move(const float& aSpeed, const float& aMaxSpeed)
{
	if (!myIsActive)
		return;
	// Animations not in folder yet.
	const float blend = aSpeed / aMaxSpeed;
	//myAnimationComponent->BlendLerpBetween(UINT_CAST(EEnemyAnimations::MoveSlow), UINT_CAST(EEnemyAnimations::MoveFast), blend);
	myReturnToIdle = blend;
}

void CEnemyAnimationController::Attack(const bool& aTemporary)
{
	if (!myIsActive)
		return;
	myAnimationComponent->BlendToAnimation(UINT_CAST(EEnemyAnimations::Attack), 0.3f, true, aTemporary);
	myReturnToIdle = 1.0f;
}

void CEnemyAnimationController::OnDeath()
{
	if (!myIsActive)
		return;
	myAnimationComponent->BlendToAnimation(UINT_CAST(EEnemyAnimations::Death), 0.3f);
	myReturnToIdle = 1.0f;
	myIsActive = false;
}

void CEnemyAnimationController::Search(const bool& aTemporary)
{
	if (!myIsActive)
		return;
	myAnimationComponent->BlendToAnimation(UINT_CAST(EEnemyAnimations::Search), 0.3f, true, aTemporary);
	myReturnToIdle = 1.0f;
}
