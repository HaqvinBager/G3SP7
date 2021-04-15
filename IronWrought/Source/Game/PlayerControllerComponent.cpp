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
#include "PlayerComponent.h"

#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"

// TEMP
static const float gPretendObjectDistanceFromPlayer = 10.0f;// TEMP
// TEMP
static float gPretendObjectCurrentDistance = 0.0f;// TEMP

CPlayerControllerComponent::CPlayerControllerComponent(CGameObject& gameObject, const float aWalkSpeed, const float aCrouchSpeed, physx::PxUserControllerHitReport* aHitReport)
	: CComponent(gameObject)
	, mySpeed(aWalkSpeed)
	, myIsCrouching(false)
	, myWalkSpeed(aWalkSpeed)
	, myCrouchSpeed(aCrouchSpeed)
	, myIsGrounded(true)
	, myHasJumped(false)
	, myIsJumping(false)
	, myJumpHeight(0.4f)
	, myFallSpeed(0.98f)
	, myMovement( Vector3(0.0f, -0.98f, 0.0f ))
{
	INPUT_MAPPER->AddObserver(EInputEvent::MoveForward,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveBackward,	this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveLeft,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::MoveRight,		this);
	INPUT_MAPPER->AddObserver(EInputEvent::Jump, this);
	INPUT_MAPPER->AddObserver(EInputEvent::Crouch, this);
	INPUT_MAPPER->AddObserver(EInputEvent::Pull, this);
	INPUT_MAPPER->AddObserver(EInputEvent::Push, this);
	INPUT_MAPPER->AddObserver(EInputEvent::ResetEntities, this);
	INPUT_MAPPER->AddObserver(EInputEvent::SetResetPointEntities, this);

	myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(gameObject.myTransform->Position(), myColliderRadius, myColliderHeightStanding, GameObject().myTransform, aHitReport);
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
	INPUT_MAPPER->RemoveObserver(EInputEvent::ResetEntities, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::SetResetPointEntities, this);

	delete myAnimationComponentController;
	myAnimationComponentController = nullptr;
}

void CPlayerControllerComponent::Awake()
{}

void CPlayerControllerComponent::Start()
{
	myRespawnPosition = myController->GetPosition();

}

void CPlayerControllerComponent::Update()
{
#ifdef _DEBUG
	if (myCamera->IsFreeCamMode() || myCamera->IsCursorUnlocked())
		return;
#endif

	if (myLadderHasTriggered)
	{
		LadderUpdate();
	}
	else
	{
		//Move({0.0f, myMovement.y, 0.0f});
	}

	//Move(myMovement * mySpeed);

	/*if (myPlayerComponent->getIsAlive() == false)
	{
		myController->SetPosition(myRespawnPosition);
		GameObject().myTransform->Position(myController->GetPosition());

		myPlayerComponent->setIsAlive(true);
		myPlayerComponent->resetHealth();
	}*/


	//std::cout << "Velocity X: " <<  myController->GetController().getActor()->getLinearVelocity().x << "Velocity Y: " << myController->GetController().getActor()->getLinearVelocity().y << "Velocity Z: " << myController->GetController().getActor()->getLinearVelocity().z << std::endl;;
	GameObject().myTransform->Position(myController->GetPosition());
	gPretendObjectCurrentDistance = max(gPretendObjectCurrentDistance -  CTimer::Dt() * 12.0f, 0.0f);
	myAnimationComponentController->UpdateBlendValue(min(gPretendObjectCurrentDistance / gPretendObjectDistanceFromPlayer, 1.0f));
	myAnimationComponentController->Update();

#ifdef _DEBUG
	if (Input::GetInstance()->IsKeyPressed('R'))
	{
		myController->SetPosition(myRespawnPosition);
	}
#endif // _DEBUG
	ControllerUpdate();




	//myMovement = { 0,0,0 };
}

void CPlayerControllerComponent::FixedUpdate()
{
	if (myHasJumped == true)
	{
		myMovement.y = myJumpHeight;
		myHasJumped = false;
	}

	//if (myMovement.y >= -0.1f)
	//{
	//}

	if (!myIsGrounded)
	{
		myMovement.y -= myFallSpeed * CTimer::FixedDt();
	}

	if (myIsJumping == false)
	{
		myMovement.y = myMovement.y > -0.0f ? myMovement.y - myFallSpeed : myMovement.y;
	}
	Move(myMovement * mySpeed);
}

void CPlayerControllerComponent::ReceiveEvent(const EInputEvent aEvent)
{
	/*CCameraControllerComponent* cameraController = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraControllerComponent>();*/
#ifdef _DEBUG
	if (myCamera->IsFreeCamMode() || myCamera->IsCursorUnlocked())
	{
		if (aEvent == EInputEvent::SetResetPointEntities)
			myRespawnPosition = myCamera->GameObject().myTransform->WorldPosition();

		return;
	}
#endif

	float y = myMovement.y;

	switch (aEvent)
	{
		case EInputEvent::Jump:
			if (myIsGrounded == true)
			{
				myHasJumped = true;
				myIsJumping = true;
				myIsGrounded = false;
			}
			break;
		case EInputEvent::Crouch:
			Crouch();
			break;

		case EInputEvent::Pull:
		{
			myAnimationComponentController->Pull(gPretendObjectCurrentDistance, gPretendObjectDistanceFromPlayer);
			gPretendObjectCurrentDistance = min(gPretendObjectCurrentDistance + CTimer::Dt() * 24.0f, gPretendObjectDistanceFromPlayer);
		}break;

		case EInputEvent::Push:
			myAnimationComponentController->Push();
			gPretendObjectCurrentDistance = 0.0f;
			break;

		case EInputEvent::ResetEntities:
			myController->SetPosition(myRespawnPosition);
			GameObject().myTransform->Position(myController->GetPosition());
			break;

		case EInputEvent::SetResetPointEntities:
			myRespawnPosition = myController->GetPosition();
			break;

		default:break;
	}

	myMovement.y = y;

	if (myLadderHasTriggered)
	{
		myMovement.y = myMovement.z;
		std::cout << myMovement.z << std::endl;
		myMovement.z = 0.0f;
		//myMovement = { 0.f, myMovement.y,0.f };
		//Move(myMovement * mySpeed);
	}
	else
	{
		//Move(myMovement * mySpeed);
	}
	//myMovement.y = 0.f;
	//myMovement = { 0.f, myMovement.y,0.f };
}

void CPlayerControllerComponent::ControllerUpdate()
{
	Vector3 horizontal = -GameObject().myTransform->GetLocalMatrix().Right() * Input::GetInstance()->GetAxis(Input::EAxis::Horizontal);
	Vector3 vertical =	-GameObject().myTransform->GetLocalMatrix().Forward() * Input::GetInstance()->GetAxis(Input::EAxis::Vertical);
	float y = myMovement.y;
	myMovement = (horizontal + vertical) * mySpeed;
	myMovement.y = y;


	if (myIsGrounded)
	{
		if(horizontal.LengthSquared() > 0.0f || vertical.LengthSquared() > 0.0f)
			myAnimationComponentController->Walk();
			//myAnimationComponentController->Walk
		//if (myMovement.x != 0.0f || myMovement.z != 0.0f)
	}

}

void CPlayerControllerComponent::Move(Vector3 aDir)
{
	//std::cout << "Gravity: " << myMovement.y << std::endl;
	
	//ir.x = aInput.x * -myCamera->GameObject().myTransform->GetLocalMatrix().Right();
	physx::PxControllerCollisionFlags collisionflag = myController->GetController().move({ aDir.x, aDir.y, aDir.z}, 0, CTimer::FixedDt(), 0);
	
	if (collisionflag != physx::PxControllerCollisionFlag::eCOLLISION_DOWN )
	{
		myIsGrounded = false;

	}

	if (collisionflag == physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		myIsGrounded = true;
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

void CPlayerControllerComponent::ResetPlayerPosition()
{
	myController->SetPosition(myRespawnPosition);
}

CCharacterController* CPlayerControllerComponent::GetCharacterController()
{
	return myController;
}

const Vector3 CPlayerControllerComponent::GetLinearVelocity()
{
	const PxVec3 pxVec3 = myController->GetController().getActor()->getLinearVelocity();
	//const Vector3& vec3 = {pxVec3.x, pxVec3.y, pxVec3.z};
	return {pxVec3.x, pxVec3.y, pxVec3.z};
}

void CPlayerControllerComponent::LadderEnter()
{
	myLadderHasTriggered = true;
}

void CPlayerControllerComponent::LadderExit()
{
	myLadderHasTriggered = false;
}

void CPlayerControllerComponent::LadderUpdate()
{
	if (myLadderHasTriggered)
	{
		//Nuddar vi Marken?

		//Försöker vi gå neråt?
	}

	//Best�mmer n�r myIsOnladder s�tts till false

	//G�ra s� att vi g�r upp och ner f�r Ladder n�r vi trycker p� W eller S
	/*if (myIsGrounded && myMovement.LengthSquared() > 0.25f)
	{
		myIsOnLadder = false;
	}*/

	//if (Input::GetInstance()->IsKeyPressed('K'))
	//{
	//	myIsOnLadder = false;
	//}
}
