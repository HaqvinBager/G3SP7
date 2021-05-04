#pragma once
#include "Component.h"
#define PI 3.14159265f
class CAIController;
class CCharacterController;
class CGameObject;

namespace physx {
	class PxUserControllerHitReport;
}

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
		TakeDamage,

		Count
	};

public:
	CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings, physx::PxUserControllerHitReport* aHitReport);
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
	float Lerp(float a, float b, float t) const
	{
		return (1.0f - t) * a + b * t;
	}

private:
	friend class CEnemyAnimationController;

	CCharacterController* myController;
	std::vector<CAIController*> myBehaviours;
	EBehaviour myCurrentState;
	CGameObject* myEnemy;
	CGameObject* myPlayer;
	SEnemySetting mySettings;
	std::vector<Vector3> myPatrolPositions;
	Quaternion myPatrolRotation;
	Vector3 myCurrentDirection;
	float myCurrentOrientation;

	float myYaw;
	float myPitch;
	
};

