#pragma once

#include "Component.h"
#include "Observer.h"

class CGameObject;
class CPlayerControllerComponent;
class CPlayerComponent : public CComponent, public IStringObserver
{
public:

	CPlayerComponent(CGameObject& gameObject);
	~CPlayerComponent();

	void Awake() override;
	void Start() override;
	void Update() override;

	bool getIsAlive();
	void setIsAlive(bool setAlive);

	void resetHealth();

	void Receive(const SStringMessage& aMessage) override;

	void OnEnable() override;
	void OnDisable() override;

private:
	CPlayerControllerComponent* myPlayerController;

	float myHealth;
	float myMaxHealth;
	float myHealthPercentage;
	bool isAlive = true;
};

