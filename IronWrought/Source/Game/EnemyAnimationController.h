#pragma once

class CAnimationComponent;
class CEnemyAnimationController
{
public:
	enum class EEnemyAnimations
	{
		Attack = 1,
		Death,
		Idle,
		Search,
		TPose,
		Count
	};
public:
	CEnemyAnimationController();
	~CEnemyAnimationController();

	void Init(CAnimationComponent* anAnimationComponent);

	void Update();
	void UpdateBlendValue(const float aBlendValue);
	void SwitchBlendTarget(const EEnemyAnimations anAnimationToBlendTo, const EEnemyAnimations anAnimationToBlendFrom = EEnemyAnimations::Idle);

	void Idle();
	void Move(const float& aSpeed = 0.5f, const float& aMaxSpeed = 1.0f);
	void Attack(const bool& aTemporary = false);
	void OnDeath();
	void Search(const bool& aTemporary = false);

private:
	CAnimationComponent* myAnimationComponent;
	bool myIsActive;
	//Temp
	//using 2.0f as max
	float myReturnToIdle = 0.0f;
	// ! Temp
};

