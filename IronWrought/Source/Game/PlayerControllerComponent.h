#pragma once
#include "Component.h"
#include "InputObserver.h"

class CCharacterController;
class CGameObject;

class CPlayerControllerComponent : public CComponent, public IInputObserver
{
public:
	CPlayerControllerComponent(CGameObject& gameObject, const float aWalkSpeed = 0.015f, const float aCrouchSpeed = 0.0075f);
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
	// 0.6f and 1.8f from GDD values for player
	const float myColliderRadius = 0.6f * 0.5f;
	const float myColliderHeight = (1.8f * 0.5f);

	/*
		Standing:
			Camera position = 1.6f
		Crouching:
			Camera position = 0.85f;
	*/
};

