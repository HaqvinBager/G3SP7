#include "stdafx.h"
#include "BoxLightComponent.h"
#include "GameObject.h"
#include "BoxLight.h"
#include "LightFactory.h"
#include "TransformComponent.h"

CBoxLightComponent::CBoxLightComponent(CGameObject& aParent, const Vector3& aDirection, const Vector3& aColor, float anIntensity, float aRange, float aWidth, float aHeight)
	: CBehaviour(aParent)
	, myBoxLight(nullptr)
{
	myBoxLight = CLightFactory::GetInstance()->CreateBoxLight();
	myBoxLight->SetPosition(GameObject().myTransform->Position());
	myBoxLight->SetDirection(aDirection);
	myBoxLight->SetRange(aRange);
	myBoxLight->SetColor(aColor);
	myBoxLight->SetIntensity(anIntensity);
	myBoxLight->SetArea({ aWidth, aHeight });
}

CBoxLightComponent::~CBoxLightComponent()
{
}

void CBoxLightComponent::Awake()
{
}

void CBoxLightComponent::Start()
{
}

void CBoxLightComponent::Update()
{
}

void CBoxLightComponent::OnEnable()
{
}

void CBoxLightComponent::OnDisable()
{
}

CBoxLight* CBoxLightComponent::GetBoxLight() const
{
	return myBoxLight;
}
