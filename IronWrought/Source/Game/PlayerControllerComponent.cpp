#include "stdafx.h"
#include "PlayerControllerComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

#include "InputMapper.h"

CPlayerControllerComponent::CPlayerControllerComponent(CGameObject& gameObject, const float aMoveHorizontalSpeed)
	: CComponent(gameObject)
	, myHorizontalMoveSpeed(aMoveHorizontalSpeed)
{
	INPUT_MAPPER->AddObserver(EInputEvent::MoveForward,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveBackward,	this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveLeft,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveRight,		this);
}

CPlayerControllerComponent::~CPlayerControllerComponent()
{
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveForward,	this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveBackward, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveLeft,		this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveRight,	this);
}

void CPlayerControllerComponent::Awake()
{}

void CPlayerControllerComponent::Start()
{}

void CPlayerControllerComponent::Update()
{
}

void CPlayerControllerComponent::ReceiveEvent(const EInputEvent aEvent)
{
	switch (aEvent)
	{
		case EInputEvent::MoveForward:
			myMovement.z = 1.0f;
			break;
		case EInputEvent::MoveBackward:
			myMovement.z = -1.0f;
			break;
		case EInputEvent::MoveLeft:
			myMovement.x = 1.0f;
			break;
		case EInputEvent::MoveRight:
			myMovement.x = -1.0f;
			break;

		default:break;
	}
	GameObject().myTransform->Move(myMovement * myHorizontalMoveSpeed * CTimer::Dt());
	myMovement = { 0.f,0.f,0.f };
}

void CPlayerControllerComponent::UpdateHorizontalMovement()
{
	
}
