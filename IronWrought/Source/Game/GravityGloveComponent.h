#pragma once
#include "Behaviour.h"

class CTransformComponent;
class CRigidBodyComponent;

class CGravityGloveComponent : public CBehaviour
{
public:
	CGravityGloveComponent(CGameObject& aParent, CTransformComponent* aGravitySlot);
	~CGravityGloveComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Pull();
	void Push();

	void OnEnable() override;
	void OnDisable() override;
private:
	CTransformComponent* myGravitySlot;
	CRigidBodyComponent* myCurrentTarget;
};

