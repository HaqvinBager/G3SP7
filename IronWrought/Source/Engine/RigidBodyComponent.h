#pragma once
#include "Component.h"

class CRigidDynamicBody;

class CRigidBodyComponent : public CComponent
{
public:
	CRigidBodyComponent(CGameObject& aParent, bool isStatic = false);
	~CRigidBodyComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
private:
	CRigidDynamicBody* myDynamicRigidBody;
};

