#pragma once
#include "Component.h"
#include "PerlinNoise.h"

class CGameObject;
class CSpriteInstance;

class CCameraComponent : public CComponent
{
public:
	CCameraComponent(CGameObject& aParent, const float aFoV = 70.0f/*, float aNearPlane = 0.3f, float aFarPlane = 10000.0f, DirectX::SimpleMath::Vector2 aResolution = {1600.f, 900.f}*/);
	~CCameraComponent();

	void Awake() override;
	void Start() override;
	void Update() override;

	const DirectX::SimpleMath::Matrix& GetProjection() const;
	float GetFoV();

	void SetTrauma(float aValue);
	void SetStartingRotation(DirectX::SimpleMath::Vector3 aRotation);
	void SetFoV(float aFoV);

	void Fade(bool aShouldFadeIn);
	const bool IsFading() const;
	void EmplaceSprites(std::vector<CSpriteInstance*>& someSprites) const;

	const Matrix& GetViewMatrix();
	const Matrix& GetShakenMatrix() const;

	const DirectX::BoundingFrustum GetViewFrustum();

private:
	void Shake();

private:
	CSpriteInstance* myFadingPlane;
	DirectX::SimpleMath::Matrix myProjection;
	DirectX::SimpleMath::Matrix myView;
	DirectX::SimpleMath::Matrix myShakenMatrix;
	DirectX::BoundingFrustum myViewFrustum;
	
	DirectX::SimpleMath::Vector3 myStartingRotation;
	DirectX::SimpleMath::Vector3 myMaxShakeRotation;
	PerlinNoise myNoise;
	float myTrauma;
	float myShake;
	float myShakeSpeed;
	float myDecayInSeconds;
	float myShakeTimer;

	float myFadeTimer;
	float myFadeSpeed;
	bool myFadingIn;
	bool myFadingPlaneActive;

	float myFoV; 

};

