#include "stdafx.h"
#include "EnviromentLightComponent.h"
#include "GameObject.h"
#include "EnvironmentLight.h"
#include "LightFactory.h"
#include "TransformComponent.h"

CEnvironmentLightComponent::CEnvironmentLightComponent(CGameObject& aParent, std::string aCubeMapName, DirectX::SimpleMath::Vector3 aColor, float anIntensity, DirectX::SimpleMath::Vector3 aDirection)
	: CComponent(aParent)
{
	myEnvironmentLight = CLightFactory::GetInstance()->CreateEnvironmentLight(ASSETPATH("Assets/Cubemaps/" + aCubeMapName + ".dds"));
	myEnvironmentLight->SetColor(aColor);
	myEnvironmentLight->SetIntensity(anIntensity);
	//not sure if this work! We want to set the direction of this transform!
	aParent.myTransform->Transform().Forward(aDirection);
	myEnvironmentLight->SetDirection(aParent.myTransform->Transform().Forward());
	//myEnvironmentLight->SetDirection(aDirection);
	myEnvironmentLight->SetPosition({ /*20.0f*/0.0f, 20.0f, 0.0f });

	myEnvironmentLight->SetNumberOfSamples(16.0f);
	myEnvironmentLight->SetLightPower(100000.0f);
	myEnvironmentLight->SetScatteringProbability(0.001f);
	myEnvironmentLight->SetHenyeyGreensteinGValue(0.0f);
}

CEnvironmentLightComponent::~CEnvironmentLightComponent()
{
}

void CEnvironmentLightComponent::Awake()
{
}

void CEnvironmentLightComponent::Start()
{
}

void CEnvironmentLightComponent::Update()
{
}
