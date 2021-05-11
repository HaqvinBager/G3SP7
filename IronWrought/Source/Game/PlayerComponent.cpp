#include "stdafx.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include "InputMapper.h"
#include "MainSingleton.h"
#include "PostMaster.h"
#include "Canvas.h"
#include "PlayerControllerComponent.h"

#define PLAYER_HEALTH_REGEN_TIMER	5.0f

CPlayerComponent::CPlayerComponent(CGameObject& gameObject, const float& aMaxHealth) 
	: CComponent(gameObject),
	myHealth(aMaxHealth),
	myMaxHealth(aMaxHealth),
	myIsAlive(true)
	, myPlayerController(nullptr)
	, myHealthHasDecreasedTimer(PLAYER_HEALTH_REGEN_TIMER)
{
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerHealthPickup, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerSetRespawnPoint, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerRespawn, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerTakeDamage, this);
}

CPlayerComponent::~CPlayerComponent()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerHealthPickup, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerSetRespawnPoint, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerRespawn, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerTakeDamage, this);
	CMainSingleton::PostMaster().Unsubscribe("Ladder", this);
}


void CPlayerComponent::Awake()
{

}

void CPlayerComponent::Start()
{
	myPlayerController = GameObject().GetComponent<CPlayerControllerComponent>();
	assert(myPlayerController != nullptr && "Couldn't find a CPlayerControllerComponent.");
}

void CPlayerComponent::Update()
{
#ifdef _DEBUG
	//if (Input::GetInstance()->IsKeyPressed('K'))
	//{
	//	DecreaseHealth();
	//}
	//if (Input::GetInstance()->IsKeyPressed('L'))
	//{
	//	float damage = 50.0f;
	//	CMainSingleton::PostMaster().Send({ EMessageType::PlayerTakeDamage, &damage });
	//}
	//if (Input::GetInstance()->IsKeyPressed('M'))
	//{
	//	float health = 10.0f;
	//	CMainSingleton::PostMaster().Send({ EMessageType::PlayerHealthPickup, &health });
	//}
	//if (Input::GetInstance()->IsKeyPressed('N'))
	//{
	//	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayerRespawn, nullptr });
	//}
	//if (Input::GetInstance()->IsKeyPressed('B'))
	//{
	//	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayerSetRespawnPoint, nullptr });
	//}
#endif // DEBUG

	RegenerateHealth();

	if (!myIsAlive)
	{
		OnNotAlive();
	}
}

void CPlayerComponent::OnNotAlive()
{
	CMainSingleton::PostMaster().Send({ EMessageType::PlayerDied, nullptr });

	myPlayerController->ResetPlayerPosition();

	ResetHealth();
}

void CPlayerComponent::RegenerateHealth(const float& somePercent)
{
	if (myHealth >= myMaxHealth)
		return;

	myHealthHasDecreasedTimer -= CTimer::Dt();
	if (myHealthHasDecreasedTimer > 0.0f)
		return;

	IncreaseHealth(myMaxHealth * (somePercent / 100.0f) * CTimer::Dt());
}

bool CPlayerComponent::GetIsAlive()
{
	return myIsAlive;
}

void CPlayerComponent::SetIsAlive(bool setAlive)
{
	myIsAlive = setAlive;
}

void CPlayerComponent::IncreaseHealth(const float& anIncreaseBy)
{
	if (anIncreaseBy < 0.0f)
		myHealthHasDecreasedTimer = PLAYER_HEALTH_REGEN_TIMER;

	myHealth += anIncreaseBy;
	if (myHealth > myMaxHealth)
		myHealth = myMaxHealth;

	SendHealthChangedMessage();

	CheckIfAlive();
}

void CPlayerComponent::DecreaseHealth(const float& aDecreaseBy)
{
	if (aDecreaseBy > 0.0f)
		myHealthHasDecreasedTimer = PLAYER_HEALTH_REGEN_TIMER;

	myHealth -= aDecreaseBy;
	if (myHealth > myMaxHealth)
		myHealth = myMaxHealth;

	SendHealthChangedMessage();

	CheckIfAlive();
}

void CPlayerComponent::ResetHealth()
{
	myHealth = myMaxHealth;
	myIsAlive = true;
	SendHealthChangedMessage();
}


void CPlayerComponent::OnEnable()
{
	CMainSingleton::PostMaster().Subscribe("Ladder", this);
}

void CPlayerComponent::OnDisable()
{
	CMainSingleton::PostMaster().Unsubscribe("Ladder", this);
}

void CPlayerComponent::CheckIfAlive()
{
	myIsAlive = (myHealth > 0.0f);
}

inline void CPlayerComponent::SendHealthChangedMessage()
{
	float healthPercentage = CurrentHealthPercent();
	CMainSingleton::PostMaster().Send({ EMessageType::PlayerHealthChanged, &healthPercentage });
}

void CPlayerComponent::Receive(const SStringMessage& aMessage)
{
	std::string ladder = "Ladder";
	if (aMessage.myMessageType == ladder)
	{
		//bool* enter = static_cast<bool*>(aMessage.data);
		PostMaster::SBoxColliderEvenTriggerData data = *static_cast<PostMaster::SBoxColliderEvenTriggerData*>(aMessage.data);
		if (data.myState == true)
		{
			myPlayerController->LadderEnter();
		}
		else
		{
			myPlayerController->LadderExit();
		}
	}
}

void CPlayerComponent::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
		case EMessageType::PlayerHealthPickup:
		{
			if (aMessage.data)
				IncreaseHealth(*reinterpret_cast<float*>(aMessage.data));
			else
				IncreaseHealth();
		}
		break;

		case EMessageType::PlayerTakeDamage:
		{
			if (aMessage.data)
				DecreaseHealth(*reinterpret_cast<float*>(aMessage.data));
			else
				DecreaseHealth();
		}
		break;

		case EMessageType::PlayerSetRespawnPoint:
		{
			myPlayerController->SetRespawnPosition();
		}
		break;

		case EMessageType::PlayerRespawn:
		{
			myPlayerController->ResetPlayerPosition();
			ResetHealth();
		}
		break;

		default:
		break;
	}
}
