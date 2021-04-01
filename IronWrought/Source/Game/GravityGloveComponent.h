#pragma once
#include "Behaviour.h"
//#include <GravityGloveEditor.h>

class CTransformComponent;
class CRigidBodyComponent;

struct SGravityGloveSettings {
	float myPushForce;
	float myDistanceToMaxLinearVelocity;
	float myMaxPushForce;


	struct Serialized
	{
		Serialized(SGravityGloveSettings& settings)
		{
			myPushForce = &settings.myPushForce;
			myDistanceToMaxLinearVelocity = &settings.myDistanceToMaxLinearVelocity;
			myMaxPushForce = &settings.myMaxPushForce;
		}
		float* myPushForce;
		float* myDistanceToMaxLinearVelocity;
		float* myMaxPushForce;
	};

	Serialized GetData()
	{
		return Serialized(*this);
	}

};

class CGravityGloveComponent : public CBehaviour
{
	//friend class IronWroughtImGui::CGravityGloveEditor;
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


	SGravityGloveSettings& GetSettings() { return mySettings; }
	//void SetSettings(const SGravityGloveSettings& someSettings) { mySettings = someSettings; }


	SGravityGloveSettings mySettings;
private:
	CTransformComponent* myGravitySlot;
	CRigidBodyComponent* myCurrentTarget;

};


