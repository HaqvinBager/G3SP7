#include "stdafx.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include "InputMapper.h"


CPlayerComponent::CPlayerComponent(CGameObject& gameObject) 

	: CComponent(gameObject),
	myHealth(100.0f), 
	isAlive(true)
{

}

CPlayerComponent::~CPlayerComponent()
{
}

void CPlayerComponent::Awake()
{
}

void CPlayerComponent::Start()
{
}

void CPlayerComponent::Update()
{
	if (myHealth >= 0)
	{
		
	}


	if (Input::GetInstance()->IsKeyPressed('K'))
	{
		myHealth -= 20;
		std::cout << myHealth << std::endl;
	}

}
