#pragma once

#include "Component.h"

class CGameObject;
 
class CPlayerComponent : public CComponent
{
public:

	CPlayerComponent(CGameObject& gameObject);
	~CPlayerComponent();




	void Awake() override;
	void Start() override;
	void Update() override;



private:

	float myHealth;
	bool isAlive;
};

