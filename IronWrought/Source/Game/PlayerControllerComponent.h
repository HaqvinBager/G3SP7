#pragma once
#include "Component.h"
#include "InputObserver.h"

class CGameObject;
class CPlayerControllerComponent : public CComponent, public IInputObserver
{
public:
	CPlayerControllerComponent(CGameObject& gameObject, const float aMoveHorizontalSpeed = 2.5f);
	~CPlayerControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void ReceiveEvent(const EInputEvent aEvent) override;

private:
	void UpdateHorizontalMovement();

	Vector3 myMovement;
	float myHorizontalMoveSpeed;
};

