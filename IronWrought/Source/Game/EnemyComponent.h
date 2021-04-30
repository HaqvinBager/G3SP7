#pragma once
#include "Component.h"

class CAIController;
class CCharacterController;
class CGameObject;

namespace physx {
	class PxUserControllerHitReport;
	class PxShape;
}

struct SEnemySetting {
	float mySpeed;	//= 10.0f;
	float myDistance;//= 0.0f;
	float myRadius;	//= 10.0f;
	float myHealth;
};

class CEnemyComponent : public CComponent
{
public:
	enum class EBehaviour {
		Patrol,
		Seek,
		Attack,
		TakeDamage
	};

public:
	CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings, physx::PxUserControllerHitReport* aHitReport);
	~CEnemyComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	void FixedUpdate() override;
	void TakeDamage(float aDamage);
	void SetState(EBehaviour aState);
	const EBehaviour GetState()const;
	void Dead();
private:
	CCharacterController* myController;
	std::vector<CAIController*> myBehaviours;
	EBehaviour myCurrentState;
	CGameObject* myEnemy;
	CGameObject* myPlayer;
	SEnemySetting mySettings;
	float myCurrentHealth;
	
};

