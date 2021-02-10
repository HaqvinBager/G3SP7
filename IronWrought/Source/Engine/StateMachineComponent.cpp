#include "stdafx.h"
#include "StateMachineComponent.h"


void CStateMachineComponent::AddState(SState* aState)
{
	myStates.emplace_back(aState);
}




CStateMachineComponent::CStateMachineComponent(CGameObject& aParent)
	: CComponent(aParent)
{
}

CStateMachineComponent::~CStateMachineComponent()
{
}


void CStateMachineComponent::Awake()
{
}
void CStateMachineComponent::Start()
{
}

void CStateMachineComponent::Update()
{
	if (myCurrentState == nullptr)
		return;

	//myCurrentState->myObject.Update();
}