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
	, myHorizontalMoveSpeed(5.0f)
{
	INPUT_MAPPER->AddObserver(EInputEvent::MoveForward,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveBackward,	this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveLeft,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveRight,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::Jump, this);

	canJump = true;
	//myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(physx::PxControllerShapeType::eCAPSULE,gameObject.myTransform->Position(), 0.6f, 1.8f);
}

CPlayerControllerComponent::~CPlayerControllerComponent()
{
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveForward,	this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveBackward, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveLeft,		this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveRight,	this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::Jump, this);

}

void CPlayerControllerComponent::Awake()
{}

void CPlayerControllerComponent::Start()
{}

void CPlayerControllerComponent::Update()
{
	//GameObject().myTransform->Position(myController->GetPosition());
}

void CPlayerControllerComponent::ReceiveEvent(const EInputEvent aEvent)
{
	switch (aEvent)
	{
		case EInputEvent::MoveForward:
			myMovement = -GameObject().myTransform->GetLocalMatrix().Forward();
			break;
		case EInputEvent::MoveBackward:
			myMovement = -GameObject().myTransform->GetLocalMatrix().Backward();
			break;
		case EInputEvent::MoveLeft:
			myMovement = GameObject().myTransform->GetLocalMatrix().Left();
			break;
		case EInputEvent::MoveRight:
			myMovement = GameObject().myTransform->GetLocalMatrix().Right();
			break;
		case EInputEvent::Jump:
			if (canJump == true)			
				Jump();		
			break;
		default:break;
	}
	//if (jumptimer == 0)
	//{
	//	myMovement.y = -1.0f; //Gravity
	//}
	//myMovement.y = -1.0f; //Gravity
	GameObject().myTransform->MoveLocal(myMovement * myHorizontalMoveSpeed * CTimer::Dt());
	//Move(myMovement * mySpeed);
	myMovement = { 0.f,0.f,0.f };
}

void CPlayerControllerComponent::Move(Vector3 aDir)
{
	physx::PxControllerCollisionFlags collisionflag = myController->GetController().move({aDir.x, aDir.y, aDir.z}, 0, CTimer::Dt(), 0);
	if (collisionflag == physx::PxControllerCollisionFlag::eCOLLISION_DOWN) 
	{
		canJump = true;
		std::cout << "collided with ground" << std::endl;
	}
}

void CPlayerControllerComponent::Jump()
{
	std::cout << "jumped" << std::endl;
	/*float jumptimer = 0;
	jumptimer += CTimer::Dt();

	while (jumptimer < 5)
	{
		myMovement.y = 1.0f; 
	}*/


}

CCharacterController* CPlayerControllerComponent::GetCharacterController()
{
	return myController;
}

void CPlayerControllerComponent::UpdateHorizontalMovement()
{
	
}
