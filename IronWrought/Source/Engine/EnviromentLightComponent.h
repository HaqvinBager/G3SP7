#pragma once
#include "Component.h"

class CGameObject;
class CEnvironmentLight;

class CEnvironmentLightComponent : public CComponent
{
public:
	CEnvironmentLightComponent(CGameObject& aParent, std::string aCubeMapName, DirectX::SimpleMath::Vector3 aColor = { 1.f, 1.f, 1.f }, float anIntensity =  1.0f, DirectX::SimpleMath::Vector3 aDirection = { 0.f, 0.f, 1.f });
	~CEnvironmentLightComponent();
	CEnvironmentLightComponent(const CEnvironmentLightComponent&) = delete;

	void Awake() override;
	void Start() override;
	void Update() override;

	CEnvironmentLight* GetEnvironmentLight() const { return myEnvironmentLight; }

private:
	CEnvironmentLight* myEnvironmentLight;
};

