#include "stdafx.h"
#include "EnemyAnimationController.h"

#include "AnimationComponent.h"
#include "EnemyComponent.h"

#define UINT_CAST(a) static_cast<unsigned int>(a)

CEnemyAnimationController::CEnemyAnimationController()
 : myAnimationComponent(nullptr)
	//, myIsActive(true)
{}

CEnemyAnimationController::~CEnemyAnimationController()
{
	myAnimationComponent = nullptr;
}

void CEnemyAnimationController::Activate()
{
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyPatrolState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemySeekState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAttackState, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAttack, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyTakeDamage, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyDied, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyDisabled, this);
}

void CEnemyAnimationController::Deactivate()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyPatrolState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemySeekState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAttackState, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAttack, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyTakeDamage, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyDied, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyDisabled, this);
}

void CEnemyAnimationController::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::EnemyPatrolState:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnPatrol(enemy);
		}break;

		case EMessageType::EnemySeekState:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnSeek(enemy);
		}break;

		case EMessageType::EnemyAttackState:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnSeek(enemy);
		}break;

		case EMessageType::EnemyAttack:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnAttack(enemy);
		}break;

		case EMessageType::EnemyTakeDamage:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnTakeDamage(enemy);
		}break;

		case EMessageType::EnemyDied:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnDeath(enemy);
		}break;

		case EMessageType::EnemyDisabled:
		{
			CEnemyComponent* enemy = static_cast<CEnemyComponent*>(aMessage.data);
			OnDisabled(enemy);
		}break;

		default:break;
	}
}

void CEnemyAnimationController::OnPatrol(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->BlendLerpBetween(UINT_CAST(EEnemyAnimations::MoveSlow), UINT_CAST(EEnemyAnimations::MoveSlow), 0.0f);
}

void CEnemyAnimationController::OnSeek(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->BlendLerpBetween(UINT_CAST(EEnemyAnimations::MoveFast), UINT_CAST(EEnemyAnimations::MoveFast), 0.0f);
}

void CEnemyAnimationController::OnAttack(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->BlendToAnimation(UINT_CAST(EEnemyAnimations::Attack), 0.33f, true, true);
}

void CEnemyAnimationController::OnTakeDamage(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->BlendToAnimation(UINT_CAST(EEnemyAnimations::Damage), 0.25f, true, true);
}

void CEnemyAnimationController::OnDeath(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->BlendLerpBetween(UINT_CAST(EEnemyAnimations::Death), UINT_CAST(EEnemyAnimations::Death), 0.0f);
}

void CEnemyAnimationController::OnDisabled(CEnemyComponent* anEnemy)
{
	if (!anEnemy)
		return;

	CAnimationComponent* anim = anEnemy->GetComponent<CAnimationComponent>();
	if (!anim)
		return;

	anim->Enabled(false);
}
