#pragma once
#include "Component.h"
#include "InputObserver.h"

class CCharacterController;
class CGameObject;
class CCameraControllerComponent;
class CPlayerControllerComponent : public CComponent, public IInputObserver
{
public:
	CPlayerControllerComponent(CGameObject& gameObject, const float aWalkSpeed = 0.01f, const float aCrouchSpeed = 0.005f, float aJumpHeight = 0.025f);
	~CPlayerControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void ReceiveEvent(const EInputEvent aEvent) override;

	void Move(Vector3 aDir);

	void SetPlayerJumpHeight(float aJumpHeight);

	//void AddFaceMesh(CGameObject* aGameObject);
	void SetControllerPosition(const Vector3& aPos);

	void Crouch();

	CCharacterController* GetCharacterController();

private:
	CCharacterController* myController;
	// A.k.a the players eyes :U // Shortcut to access for freecam toggle, Aki 12/3/2021
	CCameraControllerComponent* myCamera;
	//CGameObject* myFaceMesh;
	Vector3 myMovement;
	float mySpeed;

	bool myCanJump;
	bool myIsJumping;
	bool myHasJumped;

	bool myIsCrouching;
	const float myWalkSpeed;
	const float myCrouchSpeed;

	float myJumpHeight;

	// 0.6f is player width from GDD
	const float myColliderRadius = 0.6f * 0.5f;
	// 1.8f is player height from GDD
	const float myColliderHeightStanding = (1.8f * 0.5f);// Lowest height player can walk under: 1.9
	const float myColliderHeightCrouched = myColliderHeightStanding - 0.85f;// Lowest height player can crouch under: 1.1
	const float myCameraPosYStanding = 1.6f * 0.5f;
	const float myCameraPosYCrouching = 0.95f * 0.5f;
	const float myCameraPosZ = -0.22f;

	/*
		Standing:
			Camera position = 1.6f
		Crouching:
			Camera position = 0.85f;
	*/
};

