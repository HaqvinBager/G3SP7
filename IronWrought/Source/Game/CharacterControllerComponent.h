#pragma once
#include "Behaviour.h"
#include <PxPhysicsAPI.h>
using namespace physx;

class CCharacterController;

class CCharacterControllerComponent : public CBehaviour
{
public:
	CCharacterControllerComponent(CGameObject& aParent, PxControllerShapeType::Enum aType = PxControllerShapeType::Enum::eCAPSULE, const Vector3& aPos = {0, 0, 0}, const float& aRadius = 0.5f, const float aHeight = 2.f);
	~CCharacterControllerComponent() override;

	CCharacterController* GetCharacterController() { return myCharacterController; }

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;
private:
	CCharacterController* myCharacterController;
};

