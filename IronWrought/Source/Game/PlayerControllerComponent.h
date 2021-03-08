#pragma once
#include "Component.h"
#include "InputObserver.h"

class CCharacterController;
class CGameObject;

class CPlayerControllerComponent : public CComponent, public IInputObserver
{
public:
	CPlayerControllerComponent(CGameObject& gameObject, const float aSpeed = 0.005f);
	~CPlayerControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void ReceiveEvent(const EInputEvent aEvent) override;

	void Move(Vector3 aDir);

	void Jump();

	CCharacterController* GetCharacterController();

private:
	void UpdateHorizontalMovement();

	CCharacterController* myController;

	Vector3 myMovement;
	float myHorizontalMoveSpeed;
	float mySpeed;
	bool canJump;
};

