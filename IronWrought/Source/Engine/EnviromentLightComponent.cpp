#include "stdafx.h"
#include "EnviromentLightComponent.h"
#include "GameObject.h"
#include "EnvironmentLight.h"
#include "LightFactory.h"
#include "TransformComponent.h"

CEnviromentLightComponent::CEnviromentLightComponent(CGameObject& aParent, DirectX::SimpleMath::Vector3 aColor, float anIntensity, DirectX::SimpleMath::Vector3 aDirection)
	: CComponent(aParent)
{
	// TODO 
	//	Step through code to look for issues. Like with mips.

	myEnvironmentLight = CLightFactory::GetInstance()->CreateEnvironmentLight(ASSETPATH("Assets/Cubemaps/indoor.dds"));
	myEnvironmentLight->SetColor(aColor);
	myEnvironmentLight->SetIntensity(anIntensity);
	//not sure if this work! We want to set the direction of this transform!
	aParent.myTransform->Transform().Forward(aDirection);
	myEnvironmentLight->SetDirection(aParent.myTransform->Transform().Forward());
	myEnvironmentLight->SetPosition({ 0.0f, 2.0f, 0.0f });
}

CEnviromentLightComponent::~CEnviromentLightComponent()
{
}

void CEnviromentLightComponent::Awake()
{
}

void CEnviromentLightComponent::Start()
{
}

void CEnviromentLightComponent::Update()
{
}
