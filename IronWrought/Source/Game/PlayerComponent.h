#pragma once

#include "Component.h"
#include "Observer.h"

class CGameObject;
class CPlayerControllerComponent;
class CPlayerComponent : public CComponent, public IStringObserver, public IObserver
{
public:

	CPlayerComponent(CGameObject& gameObject, const float& aMaxHealth = 100.0f);
	~CPlayerComponent();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnNotAlive();

	void RegenerateHealth(const float& somePercent = 0.1f);

	bool GetIsAlive();
	void SetIsAlive(bool setAlive);

	inline const float CurrentHealth() const { return myHealth; }
	inline const float CurrentHealthPercent() const { return myHealth / myMaxHealth; }

	// Negative parameter will result in decreasing of health.
	void IncreaseHealth(const float& anIncreaseBy = 40.0f);
	// Negative parameter will result in increase of health.
	void DecreaseHealth(const float& aDecreaseBy = 12.0f);

	void ResetHealth();

	void Receive(const SStringMessage& aMessage) override;
	void Receive(const SMessage& aMessage) override;

	void OnEnable() override;
	void OnDisable() override;

private:
	void CheckIfAlive();
	inline void SendHealthChangedMessage();

private:
	CPlayerControllerComponent* myPlayerController;
	float myHealth;
	const float myMaxHealth;
	float myHealthHasDecreasedTimer;

	bool myIsAlive;
};

