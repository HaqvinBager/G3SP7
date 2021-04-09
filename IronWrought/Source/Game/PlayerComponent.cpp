#include "stdafx.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include "InputMapper.h"
#include "MainSingleton.h"
#include "PostMaster.h"
#include "Canvas.h"
#include "PlayerControllerComponent.h"





CPlayerComponent::CPlayerComponent(CGameObject& gameObject) 

	: CComponent(gameObject),
	myHealth(100.0f), 
	myMaxHealth(100.0f),
	isAlive(true)
{
	//CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerHealthChanged, this);
}

CPlayerComponent::~CPlayerComponent()
{
	//CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerHealthChanged, this);

}


void CPlayerComponent::Awake()
{
}

void CPlayerComponent::Start()
{
	myPlayerController = GameObject().GetComponent<CPlayerControllerComponent>();
}

void CPlayerComponent::Update()
{

	//if (Input::GetInstance()->IsKeyPressed('K'))
	//{
	//	myHealth -= 20;
	//	//std::cout << myHealth << std::endl;
	//	myHealthPercentage = (myHealth / myMaxHealth);
	//	CMainSingleton::PostMaster().Send({ EMessageType::PlayerHealthChanged, &myHealthPercentage });
	//}

	if (myHealth <= 0)
	{
		setIsAlive(false);

		CMainSingleton::PostMaster().Send({ EMessageType::PlayerDied, nullptr });

		myPlayerController->ResetPlayerPosition();

		resetHealth();
		setIsAlive(true);

	}
}



bool CPlayerComponent::getIsAlive()
{
	return isAlive;
}

void CPlayerComponent::setIsAlive(bool setAlive)
{
	isAlive = setAlive;
}

void CPlayerComponent::resetHealth()
{
	myHealth = 100.0f;
}
