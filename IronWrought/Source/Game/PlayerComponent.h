#pragma once

#include "Component.h"

class CGameObject;
class CPlayerControllerComponent;
class CPlayerComponent : public CComponent
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

private:
	CPlayerControllerComponent* myPlayerController;

	float myHealth;
	bool isAlive = true;
};

