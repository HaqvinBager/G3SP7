#pragma once
#include "Component.h"
#include "InputObserver.h"
#include "Observer.h"
class CRigidBodyComponent;
class CCharacterController;
class CGameObject;
class CCameraControllerComponent;
class CPlayerAnimationController;
class CPlayerComponent;

namespace physx {
	class PxUserControllerHitReport;
}

class CPlayerControllerComponent : public CComponent, public IInputObserver,  public IObserver, public IStringObserver
{
public:
	CPlayerControllerComponent(CGameObject& gameObject, const float aWalkSpeed = 0.314f, const float aCrouchSpeed = 0.13f, physx::PxUserControllerHitReport* aHitReport = nullptr);
	~CPlayerControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	void FixedUpdate() override;

	void ReceiveEvent(const EInputEvent aEvent) override;
	void Receive(const SStringMessage& aMsg) override;
	void Receive(const SMessage& aMsg) override;

	void ControllerUpdate();

	void Move(Vector3 aDir);

	//void AddFaceMesh(CGameObject* aGameObject);
	void SetControllerPosition(const Vector3& aPos);
	// No lerp. Instant crouch
	void Crouch();
	// Lerp for crouch
	void CrouchUpdate(const float& dt);
	void OnCrouch();

	void ResetPlayerPosition();

	CCharacterController* GetCharacterController();

	const Vector3 GetLinearVelocity();

	const float WalkSpeed() const { return myWalkSpeed; }
	void WalkSpeed(const float aSpeed) { myWalkSpeed = aSpeed; }
	const float CrouchSpeed() const { return myCrouchSpeed; }
	void CrouchSpeed(const float aSpeed) { myCrouchSpeed = aSpeed; }
	const float JumpHeight() const { return myJumpHeight; }
	void JumpHeight(const float aHeight) { myJumpHeight = aHeight; }
	const float FallSpeed() const { return myFallSpeed; }
	void FallSpeed(const float aSpeed) { myFallSpeed = aSpeed; }

	void LadderEnter();
	void LadderExit();

	void SetRespawnPosition();

private:
	void LockMovementFor(const float& someSeconds);
	void UpdateMovementLock();
	void InitForceForward();
	void UpdateForceForward();
	void InitStandStill(const float& aStandStillTimer);
	void UpdateStandStill();

	void BoundsCheck();
	void LadderUpdate();

	void UpdateEndEvent();

private:
	enum class EPlayerMovementLock
	{
		None,
		ForceFoward,
		ForceStandStill,
	};


	CCharacterController* myController;
	CPlayerAnimationController* myAnimationComponentController;
	CPlayerComponent* myPlayerComponent;
	// A.k.a the players eyes :U // Shortcut to access for freecam toggle, Aki 12/3/2021
	CCameraControllerComponent* myCamera;
	//CGameObject* myFaceMesh;
	Vector3 myRespawnPosition;
	Vector3 myMovement;
	float mySpeed;

	bool myIsGrounded;
	bool myIsJumping;
	bool myHasJumped;
	bool myLadderHasTriggered;
	bool myCanStand;

	EPlayerMovementLock myPlayerMovementLock;
	bool myIsCrouching;
	float myCrouchingLerp;
	float myWalkSpeed;
	float myCrouchSpeed;
	float myJumpHeight;
	float myFallSpeed;
	float myAirborneTimer;
	float myStepTimer;
	float myMovementLockTimer; 

	float myStepTime;

	enum class EEndEventState
	{
		Disabled,
		Active,
		AfterFall,
		End
	};
	float myEndEventTimer;
	EEndEventState myEndEvent = EEndEventState::Disabled;

	//CRigidBodyComponent* myLadder;

	// 0.6f is player width from GDD
	const float myColliderRadius = 0.6f * 0.5f;
	// 1.8f is player height from GDD
	const float myColliderHeightStanding = (1.8f * 0.5f);// Lowest height player can walk under: 1.9
	const float myColliderHeightCrouched = myColliderHeightStanding - 0.85f;// Lowest height player can crouch under: 1.1
	const float myCameraPosYStanding = 1.6f * 0.5f;
	const float myCameraPosYCrouching = 0.95f * 0.5f;
	const float myCameraPosZ = -0.22f;
	const float myMaxFallSpeed = -0.36f;

	/*
		Standing:
			Camera position = 1.6f
		Crouching:
			Camera position = 0.85f;
	*/
};
