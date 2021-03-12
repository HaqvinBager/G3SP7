#pragma once
#include "Component.h"
#include "InputObserver.h"

class CCharacterController;
class CGameObject;

class CPlayerControllerComponent : public CComponent, public IInputObserver
{
public:
	CPlayerControllerComponent(CGameObject& gameObject, const float aWalkSpeed = 0.008f, const float aCrouchSpeed = 0.004f);
	~CPlayerControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void ReceiveEvent(const EInputEvent aEvent) override;

	void Move(Vector3 aDir);

	//void AddFaceMesh(CGameObject* aGameObject);
	void SetControllerPosition(const Vector3& aPos);
	void Crouch();

	CCharacterController* GetCharacterController();

private:
	CCharacterController* myController;
	//CGameObject* myFaceMesh;
	Vector3 myMovement;
	float mySpeed;

	bool myCanJump;
	bool myIsJumping;
	bool myHasJumped;

	bool myIsCrouching;
	const float myWalkSpeed;
	const float myCrouchSpeed;

	// 0.6f is player width from GDD
	const float myColliderRadius = 0.6f * 0.5f;
	// 1.8f is player height from GDD
	const float myColliderHeightStanding = (1.8f * 0.5f);
	const float myColliderHeightCrouched = myColliderHeightStanding - 0.75f;
	const float myCameraPosYStanding = 1.6f * 0.5f;
	const float myCameraPosYCrouching = 0.85f * 0.5f;
	const float myCameraPosZ = -0.22f;

	/*
		Standing:
			Camera position = 1.6f
		Crouching:
			Camera position = 0.85f;
	*/
};

