#include "stdafx.h"
#include "PlayerControllerComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

#include "InputMapper.h"

#include "PhysXWrapper.h"
#include "CharacterController.h"

CPlayerControllerComponent::CPlayerControllerComponent(CGameObject& gameObject, const float aSpeed)
	: CComponent(gameObject)
	, mySpeed(aSpeed)
{
	INPUT_MAPPER->AddObserver(EInputEvent::MoveForward,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveBackward,	this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveLeft,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveRight,		this);

	myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(gameObject.myTransform->Position());
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
	GameObject().myTransform->Position(myController->GetPosition());
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
			myMovement.x = -1.0f;
			break;
		case EInputEvent::MoveRight:
			myMovement.x = 1.0f;
			break;

		default:break;
	}

	myMovement.y = -1.0f; //Gravity
	//GameObject().myTransform->Move(myMovement * myHorizontalMoveSpeed * CTimer::Dt());
	Move(myMovement * mySpeed);
	myMovement = { 0.f,0.f,0.f };
}

void CPlayerControllerComponent::Move(Vector3 aDir)
{
	physx::PxControllerCollisionFlags collisionflag = myController->GetController().move({aDir.x, aDir.y, aDir.z}, 0, CTimer::Dt(), 0);
	if (collisionflag == physx::PxControllerCollisionFlag::eCOLLISION_DOWN) {
	}
}

void CPlayerControllerComponent::UpdateHorizontalMovement()
{
	
}
