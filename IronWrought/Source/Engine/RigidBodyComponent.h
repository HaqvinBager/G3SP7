#pragma once
#include "Component.h"

class RigidDynamicBody;

class CRigidBodyComponent : public CComponent
{
public:
	CRigidBodyComponent(CGameObject& aParent, bool isStatic = false);
	~CRigidBodyComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
private:
	RigidDynamicBody* myDynamicRigidBody;
};

