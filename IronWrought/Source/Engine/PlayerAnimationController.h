#pragma once
#include "AnimationComponent.h"

class CAnimationComponent;
class CPlayerAnimationController
{
public:
	/*
	* EPlayerAnimations represents the fbx:es inside the Graphics/Characters/Main_Character/Animations folder.
	* Should always start on 1.
	*
	* Todo:
	*	Make it just read the files in the folder and create the necessary indexes. Would be cool atleast.
	* 
	*/
	enum class EPlayerAnimations
	{
		Idle = 1,
		Pull,
		PullHover,
		Push,
		TakingDamage,
		TPose,
		Walk,
		Count
	};
public:
	// This is freaking disaster

	CPlayerAnimationController();
	~CPlayerAnimationController();
	
	void Init(CAnimationComponent* anAnimationComponent);

	void Update();
	void UpdateBlendValue(const float aBlendValue);
	void SwitchBlendTarget(const EPlayerAnimations anAnimationToBlendTo, const EPlayerAnimations anAnimationToBlendFrom = EPlayerAnimations::Idle);

	void Idle();
	void Walk();
	void Pull(const float anObjectDistanceToPlayer, const float anInitialDistance);
	void PullHoldingObject();
	void Push();
	void TakeDamage();

private:
	CAnimationComponent* myPlayerAnimation;
	//Temp
	//using 2.0f as max
	float myReturnToIdle = 0.0f;
	// ! Temp
};

