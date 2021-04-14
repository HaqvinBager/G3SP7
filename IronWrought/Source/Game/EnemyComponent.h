#pragma once
#include "Component.h"
#define PI 3.14159265f
class CAIController;
class CCharacterController;
class CGameObject;

struct SEnemySetting {
	float mySpeed;	//= 0.1f;
	float myRadius;	//= 10.0f;
	float myHealth; //= 10.0f
	float myAttackDistance; //2.0f

	std::vector<int> myPatrolGameObjectIds;
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
	CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings);
	~CEnemyComponent() override;

	void Awake() override;
	void Start() override;
	void Update()override;
	void TakeDamage();
	void SetState(EBehaviour aState);
	const EBehaviour GetState()const;


public:
	float WrapAngle(float anAngle)
	{
		return fmodf(anAngle, 360.0f);
	}

	float ToDegrees(float anAngleInRadians)
	{
		return anAngleInRadians * (180.0f / PI);
	}


private:
	CCharacterController* myController;
	std::vector<CAIController*> myBehaviours;
	EBehaviour myCurrentState;
	CGameObject* myEnemy;
	CGameObject* myPlayer;
	SEnemySetting mySettings;
	std::vector<Vector3> myPatrolPositions;
	Quaternion myPatrolRotation;
	float myYaw;
	float myPitch;
	
};

