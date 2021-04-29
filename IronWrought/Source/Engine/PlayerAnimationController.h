#pragma once
#include "AnimationComponent.h"
#include "Observer.h"

class CAnimationComponent;
class CPlayerAnimationController : public IObserver
{
public:
	/*
	* EPlayerAnimations represents the fbx:es inside the Graphics/Characters/Main_Character/Animations folder.
	* Should always start on 1.
	*
	* Todo:
	*	Make it just read the files in the folder and create the necessary indices. Would be cool atleast.
	* 
	*/
	enum class EPlayerAnimations
	{
		None = 0,
		Idle = 1,
		Pull,
		PullHover,
		Push,
		TakingDamage,
		TPose,
		Walk,

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
	void Receive(const SMessage& aMessage) override;

public:
	// This is freaking disaster

	CPlayerAnimationController();
	~CPlayerAnimationController();
	
	void Init(CAnimationComponent* anAnimationComponent);

	void Update(const Vector3& aMovement);

	void Walk(const Vector3& aMovement);
	void Pull();
	void Push();
	void TakeDamage();

private:
	CAnimationComponent* myPlayerAnimation;
	EState myState;
};

