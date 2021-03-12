#include "stdafx.h"
#include "PlayerControllerComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

#include "InputMapper.h"

#include "PhysXWrapper.h"
#include "CharacterController.h"

#include "Scene.h"
#include "CameraControllerComponent.h"

CPlayerControllerComponent::CPlayerControllerComponent(CGameObject& gameObject, const float aWalkSpeed, const float aCrouchSpeed)
	: CComponent(gameObject)
	, mySpeed(aWalkSpeed)
	, myIsCrouching(false)
	, myWalkSpeed(aWalkSpeed)
	, myCrouchSpeed(aCrouchSpeed)
	, myCanJump(true)
	, myHasJumped(false)
	, myIsJumping(false)
	, myMovement( Vector3(0.0f, -0.098f, 0.0f ))
{
	INPUT_MAPPER->AddObserver(EInputEvent::MoveForward,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveBackward,	this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveLeft,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveRight,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::Jump, this);
	INPUT_MAPPER->AddObserver(EInputEvent::Crouch, this);

	myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(gameObject.myTransform->Position(), myColliderRadius, myColliderHeightStanding);
	GameObject().myTransform->Position(myController->GetPosition());// This is a test / Aki 2021 03 12 

	GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYStanding, myCameraPosZ });
	GameObject().myTransform->FetchChildren()[0]->Rotation({ 0.0f, 0.0f, 0.0f });
}

CPlayerControllerComponent::~CPlayerControllerComponent()
{
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveForward,	this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveBackward, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveLeft,		this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveRight,	this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::Jump, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::Crouch, this);
}

void CPlayerControllerComponent::Awake()
{}

void CPlayerControllerComponent::Start()
{}

void CPlayerControllerComponent::Update()
{
	Move({0.0f, myMovement.y, 0.0f});
	
	if (myHasJumped == true)
	{
		myMovement.y = 0.01f;
		myHasJumped = false;
	}

	if (myMovement.y >= -1)
	{
		myMovement.y -= 0.5f * CTimer::Dt();
	}
	
	if (myIsJumping == false)
	{
		myMovement.y = -0.098f;
	}

	GameObject().myTransform->Position(myController->GetPosition());
}

void CPlayerControllerComponent::ReceiveEvent(const EInputEvent aEvent)
{
	CCameraControllerComponent* cameraController = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraControllerComponent>();

	float y = myMovement.y;

	switch (aEvent)
	{
		case EInputEvent::MoveForward:
			myMovement = -cameraController->GameObject().myTransform->GetLocalMatrix().Forward();
			break;
		case EInputEvent::MoveBackward:
			myMovement = -cameraController->GameObject().myTransform->GetLocalMatrix().Backward();
			break;
		case EInputEvent::MoveLeft:
			myMovement = cameraController->GameObject().myTransform->GetLocalMatrix().Left();
			break;
		case EInputEvent::MoveRight:
			myMovement = cameraController->GameObject().myTransform->GetLocalMatrix().Right();
			break;
		case EInputEvent::Jump:
			if (myCanJump == true)
			{
				myHasJumped = true;
				myIsJumping = true;
				myCanJump = false;
			}
			break;
		case EInputEvent::Crouch:
			Crouch();
		break;
		default:break;
	}

	myMovement.y = y;

	Move(myMovement * mySpeed);
	myMovement = { 0.f, myMovement.y,0.f };	
}

void CPlayerControllerComponent::Move(Vector3 aDir)
{
	physx::PxControllerCollisionFlags collisionflag = myController->GetController().move({aDir.x, aDir.y, aDir.z}, 0, CTimer::Dt(), 0);
	if (collisionflag == physx::PxControllerCollisionFlag::eCOLLISION_DOWN) 
	{
		myCanJump = true;
 	}
}

void CPlayerControllerComponent::SetControllerPosition(const Vector3& aPos)
{
	myController->GetController().setPosition({ aPos.x, aPos.y, aPos.x });
}

void CPlayerControllerComponent::Crouch()
{
	myIsCrouching = !myIsCrouching;
	if (myIsCrouching)
	{
		myController->GetController().resize(myColliderHeightCrouched);
		GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYCrouching, myCameraPosZ });
		mySpeed = myCrouchSpeed;
	}
	else
	{
		myController->GetController().resize(myColliderHeightStanding);
		GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYStanding, myCameraPosZ });
		mySpeed = myWalkSpeed;
	}
}

CCharacterController* CPlayerControllerComponent::GetCharacterController()
{
	return myController;
}