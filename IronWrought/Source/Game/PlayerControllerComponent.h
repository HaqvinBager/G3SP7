#pragma once
#include "Component.h"
#include "InputObserver.h"

class CCharacterController;
class CGameObject;

class CPlayerControllerComponent : public CComponent, public IInputObserver
{
public:
	CPlayerControllerComponent(CGameObject& gameObject, const float aSpeed = 0.025f);
	~CPlayerControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void ReceiveEvent(const EInputEvent aEvent) override;

	void Move(Vector3 aDir);

	void AddFaceMesh(CGameObject* aGameObject);
	void Jump();
	void SetControllerPosition(const Vector3& aPos);

	CCharacterController* GetCharacterController();

private:
	void UpdateHorizontalMovement();

	CCharacterController* myController;
	CGameObject* myFaceMesh;
	Vector3 myMovement;
	float myHorizontalMoveSpeed;
	float mySpeed;
	bool canJump;
};

