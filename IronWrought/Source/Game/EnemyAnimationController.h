#pragma once
#include "Observer.h"

class CAnimationComponent;
class CEnemyComponent;
class CEnemyAnimationController : public IObserver
{
public:
	enum class EEnemyAnimations
	{
		Attack = 1,
		Damage,
		Death,
		Idle,
		MoveFast,
		MoveSlow,
		Search,
		TPose,
		Count
	};

	enum class EState
	{
		Idle,
		Moving,
		Action,
		Transition,

		None,
		Count
	};

public:
	CEnemyAnimationController();
	~CEnemyAnimationController();

	void Activate();
	void Deactivate();
	void Receive(const SMessage& aMessage) override;

private:
	void OnPatrol(CEnemyComponent* anEnemy);
	void OnSeek(CEnemyComponent* anEnemy);
	void OnAttack(CEnemyComponent* anEnemy);
	void OnTakeDamage(CEnemyComponent* anEnemy);

	/*void Init(CAnimationComponent* anAnimationComponent);

	void Update();
	void UpdateBlendValue(const float aBlendValue);
	void SwitchBlendTarget(const EEnemyAnimations anAnimationToBlendTo, const EEnemyAnimations anAnimationToBlendFrom = EEnemyAnimations::Idle);

	void Idle();
	void Move(const float& aSpeed = 0.5f, const float& aMaxSpeed = 1.0f);
	void Attack(const bool& aTemporary = true);
	void OnDeath();
	void Search(const bool& aTemporary = true);*/

private:
	CAnimationComponent* myAnimationComponent;
	//bool myIsActive;
	////Temp
	////using 2.0f as max
	//float myReturnToIdle = 0.0f;
	//// ! Temp
};

