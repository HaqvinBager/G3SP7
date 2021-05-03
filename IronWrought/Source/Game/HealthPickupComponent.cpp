#include "stdafx.h"
#include "HealthPickupComponent.h"
#include "RigidBodyComponent.h"
#include "Engine.h"
#include "Scene.h"

CHealthPickupComponent::CHealthPickupComponent(CGameObject& aParent, const float& aHealthPickupAmount)
	: CBehaviour(aParent)
	, myHealthPickupAmount(aHealthPickupAmount)
{
}

CHealthPickupComponent::~CHealthPickupComponent()
{
	myHealthPickupAmount = 0;
}

void CHealthPickupComponent::Destroy()
{
	GameObject().GetComponent<CRigidBodyComponent>()->Destroy();
	CEngine::GetInstance()->GetActiveScene().RemoveInstance(&GameObject());
}

void CHealthPickupComponent::Awake()
{
}

void CHealthPickupComponent::Start()
{
}

void CHealthPickupComponent::Update()
{
}

void CHealthPickupComponent::OnEnable()
{
	Enabled(true);
}

void CHealthPickupComponent::OnDisable()
{
	Enabled(false);
}
