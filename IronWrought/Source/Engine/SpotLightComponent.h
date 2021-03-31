#pragma once
#include "Behaviour.h"

class CGameObject;
class CSpotLight;

class CSpotLightComponent : public CBehaviour
{
public:
	CSpotLightComponent(CGameObject& aParent, Vector3 aColor, float anIntensity, Vector3 aDirection, float aRange, float aWideness);
	~CSpotLightComponent();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

	CSpotLight* GetSpotLight() const;

private:
	CSpotLight* mySpotLight;
	Vector3 myColor;
	Vector3 myDirection;
	float myIntensity;
	float myRange;
	float myWideness;
};

