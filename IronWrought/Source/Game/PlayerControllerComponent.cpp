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
#include "ModelComponent.h"

#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"

#define PLAYER_MAX_POSITION 500.0f
#define PLAYER_MIN_POSITION -500.0f

CPlayerControllerComponent::CPlayerControllerComponent(CGameObject& gameObject, const float aWalkSpeed, const float aCrouchSpeed, physx::PxUserControllerHitReport* aHitReport)
	: CComponent(gameObject)
	, mySpeed(aWalkSpeed)
	, myIsCrouching(false)
	, myWalkSpeed(aWalkSpeed)
	, myCrouchSpeed(aCrouchSpeed)
	, myIsGrounded(true)
	, myHasJumped(false)
	, myIsJumping(false)
	, myJumpHeight(0.1f)// these values don't make sense. //Supposed to be 40cm => ~0.4f
	, myFallSpeed(0.982f * 1.0f)
	, myMovement(Vector3(0.0f, 0.0f, 0.0f))
	, myAirborneTimer(0.0f)
	, myLadderHasTriggered(false)
	, myAnimationComponentController(nullptr)
	, myPlayerComponent(nullptr)
	, myStepTimer(0.0f)
	, myLockPlayerInput(false)
	, myMovementLockTimer(0.0f)
	, myEventCounter(0)
	, myStepTime(aWalkSpeed * 5.0f)
	, myCanStand(true)
{
	INPUT_MAPPER->AddObserver(EInputEvent::Jump, this);
	INPUT_MAPPER->AddObserver(EInputEvent::Crouch, this);
	INPUT_MAPPER->AddObserver(EInputEvent::ResetEntities, this);
	INPUT_MAPPER->AddObserver(EInputEvent::SetResetPointEntities, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayerTakeDamage, this);

	myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(gameObject.myTransform->Position(), myColliderRadius, myColliderHeightStanding, GameObject().myTransform, aHitReport);
	physx::PxShape* shape = nullptr;
	myController->GetController().getActor()->getShapes(&shape, 1);
	shape->setFlag(PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, true);
	
	PxFilterData filterData;
	filterData.word0 = CPhysXWrapper::ELayerMask::PLAYER;
	shape->setQueryFilterData(filterData);

	GameObject().myTransform->Position(myController->GetPosition());// This is a test / Aki 2021 03 12

	GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYStanding, myCameraPosZ });
	GameObject().myTransform->FetchChildren()[0]->Rotation({ 0.0f, 0.0f, 0.0f });
	myCamera = GameObject().myTransform->FetchChildren()[0]->GameObject().GetComponent<CCameraControllerComponent>();

	CAnimationComponent* animComp = GameObject().myTransform->FetchChildren()[0]->GameObject().GetComponent<CAnimationComponent>();
	if (animComp)
	{
		myAnimationComponentController = new CPlayerAnimationController();
		myAnimationComponentController->Init(animComp);
	}
	else
	{
		assert(false && "No animation component available!");
	}
}

CPlayerControllerComponent::~CPlayerControllerComponent()
{
	INPUT_MAPPER->RemoveObserver(EInputEvent::Jump, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::Crouch, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::ResetEntities, this);
	INPUT_MAPPER->RemoveObserver(EInputEvent::SetResetPointEntities, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayerTakeDamage, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_DISABLE_GLOVE, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_ENABLE_GLOVE, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_OUTRO1, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_OUTRO2, this);

	delete myAnimationComponentController;
	myAnimationComponentController = nullptr;
}

void CPlayerControllerComponent::Awake()
{}

void CPlayerControllerComponent::Start()
{
	SetRespawnPosition();
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_DISABLE_GLOVE, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_ENABLE_GLOVE, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_OUTRO1, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_OUTRO2, this);
}

void CPlayerControllerComponent::Update()
{
#ifdef _DEBUG
	if (myCamera->IsFreeCamMode() || myCamera->IsCursorUnlocked())
		return;
#endif

	GameObject().myTransform->Position(myController->GetPosition());
	myAnimationComponentController->Update(myMovement);

	if (myLockPlayerInput)
		OnInputLockUpdate();
	else
		ControllerUpdate();

	UpdateMovementLock();

	BoundsCheck();
	

#ifdef _DEBUG
	if (Input::GetInstance()->IsKeyPressed('R'))
	{
		ResetPlayerPosition();
	}
#endif // _DEBUG
}

void CPlayerControllerComponent::FixedUpdate()
{
	if (myLadderHasTriggered)
	{
		LadderUpdate();
	}
	else
	{
		if (myHasJumped == true)
		{
			myMovement.y = myJumpHeight;
			myAirborneTimer = 0.0f;
			myHasJumped = false;
			CMainSingleton::PostMaster().SendLate({ EMessageType::PlayJumpSound, nullptr });
		}

		myMovement.y -= myFallSpeed * myFallSpeed * CTimer::FixedDt() * myAirborneTimer ;
		if (!myIsGrounded) {
			myAirborneTimer += CTimer::FixedDt();
		}

		if (myMovement.y < myMaxFallSpeed)
			myMovement.y = myMaxFallSpeed;

		Move({ myMovement.x, myMovement.y, myMovement.z });
		CrouchUpdate(CTimer::FixedDt() * 2.0f);
	}
}

void CPlayerControllerComponent::ReceiveEvent(const EInputEvent aEvent)
{
	if (myLockPlayerInput)
	{
		OnInputLockEvent();
		return;
	}

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
			OnCrouch();
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
}

void CPlayerControllerComponent::Receive(const SStringMessage& aMsg)
{
	if (PostMaster::DisableGravityGlove(aMsg.myMessageType))
	{
		myCamera->GameObject().GetComponent<CModelComponent>()->Enabled(false);
		myCamera->GameObject().GetComponent<CAnimationComponent>()->Enabled(false);
		return;
	}

	if (PostMaster::EnableGravityGlove(aMsg.myMessageType))
	{
		myCamera->GameObject().GetComponent<CModelComponent>()->Enabled(true);
		myCamera->GameObject().GetComponent<CAnimationComponent>()->Enabled(true);
		return;
	}

	//switch (myEventCounter)
	//{
	//	case 0:
	//	// maybe create a switch case to reduce string comparisons.
	//	break;
	//}

	if (PostMaster::CompareStringMessage(PostMaster::SMSG_OUTRO1, aMsg.myMessageType))
	{
		if (myEventCounter > 0)
			return;
		myEventCounter++;

		CMainSingleton::PostMaster().Send({ PostMaster::SMSG_DISABLE_GLOVE, nullptr });
		myLockPlayerInput = true;

		PostMaster::SBoxColliderEvenTriggerData data = *static_cast<PostMaster::SBoxColliderEvenTriggerData*>(aMsg.data);
		CTransformComponent* transform = data.myTransform;
		GameObject().myTransform->CopyRotation(transform->Transform());

		return;
	}

	if (PostMaster::CompareStringMessage(PostMaster::SMSG_OUTRO2, aMsg.myMessageType))
	{
		if (myEventCounter > 1)
			return;
		myEventCounter++;

		PostMaster::SBoxColliderEvenTriggerData data = *static_cast<PostMaster::SBoxColliderEvenTriggerData*>(aMsg.data);
		CTransformComponent* transform = data.myTransform;
		GameObject().myTransform->CopyRotation(transform->Transform());
		
		LockMovementFor(5.0f);

		return;
	}

	if (PostMaster::CompareStringMessage(PostMaster::SMSG_OUTRO3, aMsg.myMessageType))
	{
		if (myEventCounter > 2)
			return;
		myEventCounter++;

		PostMaster::SBoxColliderEvenTriggerData data = *static_cast<PostMaster::SBoxColliderEvenTriggerData*>(aMsg.data);
		CTransformComponent* transform = data.myTransform;
		GameObject().myTransform->CopyRotation(transform->Transform());

		LockMovementFor(5.0f);

		return;
	}
}

void CPlayerControllerComponent::Receive(const SMessage& aMsg)
{
	if (aMsg.myMessageType == EMessageType::PlayerTakeDamage)
	{
		myAnimationComponentController->TakeDamage();
	}
}

void CPlayerControllerComponent::ControllerUpdate()
{
	const float horizontalInput = Input::GetInstance()->GetAxis(Input::EAxis::Horizontal);
	const float verticalInput = Input::GetInstance()->GetAxis(Input::EAxis::Vertical);
	Vector3 horizontal = -GameObject().myTransform->GetLocalMatrix().Right() * horizontalInput;
	Vector3 vertical =	-GameObject().myTransform->GetLocalMatrix().Forward() * verticalInput;
	float y = myMovement.y;
	myMovement = (horizontal + vertical) * mySpeed;
	myMovement.y = y;
}

void CPlayerControllerComponent::Move(Vector3 aDir)
{
	physx::PxControllerCollisionFlags collisionflag = myController->GetController().move({ aDir.x, aDir.y, aDir.z}, 0, CTimer::FixedDt(), 0);
	
	if (!myIsGrounded && (collisionflag & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)) 
	{
		CMainSingleton::PostMaster().SendLate({ EMessageType::PlayStepSound, nullptr }); // Landing
	}
	
	myIsGrounded = (collisionflag & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);
	if (myIsGrounded)
	{
		myAirborneTimer = 0.f;

		if (myLockPlayerInput)
			return;

		Vector2 horizontalDir(aDir.x, aDir.z);
		if (horizontalDir.LengthSquared() > 0.0f)
		{
			myStepTimer -= CTimer::FixedDt();
			if (myStepTimer <= 0.0f && !myIsCrouching) 
			{
				myStepTimer = myStepTime;
				CMainSingleton::PostMaster().SendLate({ EMessageType::PlayStepSound, nullptr });
			}
		}
	}
}

void CPlayerControllerComponent::SetControllerPosition(const Vector3& aPos)
{
	myController->GetController().setPosition({ aPos.x, aPos.y, aPos.x });
}

inline const float Lerp(const float& A, const float& B, const float& T)
{
	return ((A - A * T) + (B * T));
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
		//myAnimationComponentController->TakeDamage();// TEMP :)
		// SUPER TEMP :)
	}
	else
	{
		myController->GetController().resize(myColliderHeightStanding);
		GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, myCameraPosYStanding, myCameraPosZ });// Equivalent to myCamera->GameObject().myTransform->Position
		mySpeed = myWalkSpeed;
	}
}

void CPlayerControllerComponent::CrouchUpdate(const float& dt)
{
	if (myCrouchingLerp >= 0.0f && myCrouchingLerp <= 1.0f)
	{
		float currentYPos = Lerp(myCameraPosYStanding, myCameraPosYCrouching, myCrouchingLerp);
		GameObject().myTransform->FetchChildren()[0]->Position({ 0.0f, currentYPos, myCameraPosZ });// Equivalent to myCamera->GameObject().myTransform->Position
	}
	else
	{
		return;
	}

	if(myIsCrouching && myCanStand)
		myCrouchingLerp += dt;
	else
		myCrouchingLerp -= dt;
}

void CPlayerControllerComponent::OnCrouch()
{
	if (myCanStand) {
		myIsCrouching = !myIsCrouching;
	}
	if (myIsCrouching)
	{
		myController->GetController().resize(myColliderHeightCrouched);
		mySpeed = myCrouchSpeed;
		myCrouchingLerp = 0.0f;
	}
	else
	{
		Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
		start.y += (myColliderHeightStanding / 2);
		Vector3 dir = GameObject().myTransform->GetWorldMatrix().Up();
		//checks if we can stand up 
		PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, (myColliderHeightStanding), CPhysXWrapper::ELayerMask::STATIC_ENVIRONMENT);
		if (hit.getNbAnyHits() <= 0) {
			myCanStand = true;
			myController->GetController().resize(myColliderHeightStanding);
			mySpeed = myWalkSpeed;
			myCrouchingLerp = 1.0f;
		}
		else {
			myCanStand = false;
		}
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

void CPlayerControllerComponent::SetRespawnPosition()
{
	myRespawnPosition = myController->GetPosition();
}	

void CPlayerControllerComponent::LockMovementFor(const float& someSeconds)
{
	myMovementLockTimer = someSeconds;
	myMovement = { 0.0f, myMovement.y, 0.0f };
}

void CPlayerControllerComponent::UpdateMovementLock()
{
	if (myMovementLockTimer < 0.0f)
		return;

	myMovementLockTimer -= CTimer::Dt();
	myMovement = { 0.0f, myMovement.y, 0.0f };
}

void CPlayerControllerComponent::OnInputLockEvent()
{
	Vector3 vertical =	-GameObject().myTransform->GetLocalMatrix().Forward();
	float y = myMovement.y;
	myMovement = (vertical) * mySpeed;
	myMovement.y = y;
}

void CPlayerControllerComponent::OnInputLockUpdate()
{
	const float horizontalInput = Input::GetInstance()->GetAxis(Input::EAxis::Horizontal);
	const float verticalInput = Input::GetInstance()->GetAxis(Input::EAxis::Vertical);

	if (horizontalInput != 0.0f || verticalInput != 0.0f)
	{
		Vector3 vertical =	-GameObject().myTransform->GetLocalMatrix().Forward();
		float y = myMovement.y;
		myMovement = (vertical) * mySpeed;
		myMovement.y = y;
	}
	else
	{
		Vector3 vertical =	-GameObject().myTransform->GetLocalMatrix().Forward();
		float y = myMovement.y;
		myMovement = (vertical) * mySpeed; //* 0.5f;
		myMovement.y = y;
	}
}

void CPlayerControllerComponent::BoundsCheck()
{
	const Vector3 playerPos = GameObject().myTransform->Position();
	if ((playerPos.y < PLAYER_MAX_POSITION && playerPos.y > PLAYER_MIN_POSITION) == false)
		ResetPlayerPosition();
}

void CPlayerControllerComponent::LadderUpdate()
{
	if (Input::GetInstance()->IsKeyDown('W'))
	{
		Move({ 0.0f, 0.075f, 0.0f });
	}
	else if (Input::GetInstance()->IsKeyDown('S'))
	{
		Move({ 0.0f, -0.075f, 0.0f });
	}

	if (myIsGrounded)
	{
		if (Input::GetInstance()->IsKeyDown('S'))
		{
			LadderExit();
		}
	}

	//if (myHasJumped)
	//{
	//	LadderExit();
	//}

	//	std::cout << myMovement.z << std::endl;
	//	if (myMovement.z < 0.0f)
	//	{
	//		LadderExit();
	//	}
	//	//std::cout << "Touched Ground" << std::endl;
	//}
	//else
	//{
	//	Move({ 0.0f, myMovement.z, 0.0f });
	//}



	//if (myLadderHasTriggered)
	//{
		//Nuddar vi Marken?

		//Försöker vi gå neråt?
	//}

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
