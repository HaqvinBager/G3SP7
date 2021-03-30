#pragma once
#include "Component.h"

class CAIController;
class CCharacterController;
class CGameObject;
class CEnemyComponent : public CComponent
{
public:
	enum class EBehaviour {
		Patrol,
		Seek,
		Attack
	};

public:
	CEnemyComponent(CGameObject& aParent);
	~CEnemyComponent() override;

	void Awake() override;
	void Start() override;
	void Update()override;
	void SetState(EBehaviour aState);
	const EBehaviour GetState()const;
private:
	CCharacterController* myController;
	std::vector<CAIController*> myBehaviours;
	EBehaviour myCurrentState;
	float mySpeed;
	CGameObject* myEnemy;
	CGameObject* myPlayer;
	float myDistance;
	float myRadius;
};

