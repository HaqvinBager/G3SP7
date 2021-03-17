#include "stdafx.h"
#include "PlayerControllerComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

#include "InputMapper.h"

#include "PhysXWrapper.h"
#include "CharacterController.h"

#include "Scene.h"
#include "CameraControllerComponent.h"

#include "PlayerAnimationController.h"

// TEMP
static const float gPretendObjectDistanceFromPlayer = 5.0f;// TEMP
// TEMP
static float gPretendObjectCurrentDistance = 0.0f;// TEMP

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
	INPUT_MAPPER->AddObserver(EInputEvent::Pull, this);
	INPUT_MAPPER->AddObserver(EInputEvent::Push, this);

	myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(gameObject.myTransform->Position(), myColliderRadius, myColliderHeightStanding);
	GameObject().myTransform->Position(myController->GetPosition());// This is a test / Aki 2021 03 12 

	GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYStanding, myCameraPosZ });
	GameObject().myTransform->FetchChildren()[0]->Rotation({ 0.0f, 0.0f, 0.0f });
	myCamera = GameObject().myTransform->FetchChildren()[0]->GameObject().GetComponent<CCameraControllerComponent>();

	myAnimationComponentController = new CPlayerAnimationController();
	CAnimationComponent* animComp = GameObject().myTransform->FetchChildren()[0]->FetchChildren()[0]->GameObject().GetComponent<CAnimationComponent>();
	myAnimationComponentController->Init(animComp);
}

CPlayerControllerComponent::~CPlayerControllerComponent()
{
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveForward,	this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveBackward, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveLeft,		this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::MoveRight,	this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::Jump, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::Crouch, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::Pull, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::Push, this);

	delete myAnimationComponentController;
	myAnimationComponentController = nullptr;
}

void CPlayerControllerComponent::Awake()
{}

void CPlayerControllerComponent::Start()
{}

void CPlayerControllerComponent::Update()
{
#ifdef _DEBUG
	if (myCamera->IsFreeCamMode())
		return;
#endif
	Move({0.0f, myMovement.y, 0.0f});
	//Move(myMovement * mySpeed);
	
	if (myHasJumped == true)
	{
		myMovement.y = 0.035f;
		myHasJumped = false;
	}

	if (myMovement.y >= -1)
	{
		myMovement.y -= 0.1f * CTimer::Dt();
	}
	
	if (myIsJumping == false)
	{
		myMovement.y = -0.098f;
	}

	GameObject().myTransform->Position(myController->GetPosition());
	gPretendObjectCurrentDistance = max(gPretendObjectCurrentDistance -  CTimer::Dt() * 12.0f, 0.0f);
	myAnimationComponentController->UpdateBlendValue(min(gPretendObjectCurrentDistance / gPretendObjectDistanceFromPlayer, 1.0f));

	myAnimationComponentController->Update();
}

void CPlayerControllerComponent::ReceiveEvent(const EInputEvent aEvent)
{
	/*CCameraControllerComponent* cameraController = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraControllerComponent>();*/
#ifdef _DEBUG
	if (myCamera->IsFreeCamMode())
		return;
#endif

	if (myMovement.x != 0.0f || myMovement.z != 0.0f)
	{
		//myAnimationComponentController->Walk();
	}

	float y = myMovement.y;

	switch (aEvent)
	{
		case EInputEvent::MoveForward:
			myMovement = -myCamera->GameObject().myTransform->GetLocalMatrix().Forward();
			myAnimationComponentController->Walk();
			break;
		case EInputEvent::MoveBackward:
			myMovement = -myCamera->GameObject().myTransform->GetLocalMatrix().Backward();
			myAnimationComponentController->Walk();
			break;
		case EInputEvent::MoveLeft:
			myMovement = myCamera->GameObject().myTransform->GetLocalMatrix().Left();
			myAnimationComponentController->Walk();
			break;
		case EInputEvent::MoveRight:
			myMovement = myCamera->GameObject().myTransform->GetLocalMatrix().Right();
			myAnimationComponentController->Walk();
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

		case EInputEvent::Pull:
			myAnimationComponentController->Pull(gPretendObjectCurrentDistance, gPretendObjectDistanceFromPlayer);
			gPretendObjectCurrentDistance = min(gPretendObjectCurrentDistance + CTimer::Dt() * 24.0f, gPretendObjectDistanceFromPlayer);
			break;
		case EInputEvent::Push:
			myAnimationComponentController->Push();
			gPretendObjectCurrentDistance = 0.0f;
			break;

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
		if(aDir.x != 0.0f || aDir.z != 0.0f)
			myAnimationComponentController->Walk();
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
		GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYCrouching, myCameraPosZ });// Equivalent to myCamera->GameObject().myTransform->Position
		mySpeed = myCrouchSpeed;
		// THIS IS TEMP :)
		myAnimationComponentController->TakeDamage();// TEMP :)
		// SUPER TEMP :)
	}
	else
	{
		myController->GetController().resize(myColliderHeightStanding);
		GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYStanding, myCameraPosZ });// Equivalent to myCamera->GameObject().myTransform->Position
		mySpeed = myWalkSpeed;
	}
}

CCharacterController* CPlayerControllerComponent::GetCharacterController()
{
	return myController;
}