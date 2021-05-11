#include "stdafx.h"
#include "CameraControllerComponent.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "Scene.h"
#include <algorithm>
#include "PlayerControllerComponent.h"
#include "CharacterController.h"
#include "LineFactory.h"
#include "LineInstance.h"

#include "MainSingleton.h"
#include "PostMaster.h"



CCameraControllerComponent::CCameraControllerComponent(CGameObject& aGameObject, float aCameraMoveSpeed, ECameraMode aCameraMode, char aToggleFreeCam, Vector3 aOffset)
	: CComponent(aGameObject),
	myCameraMoveSpeed(aCameraMoveSpeed),
	myCamera(nullptr),
	myCameraMode(aCameraMode),
	myPrevCameraMode(aCameraMode),
	myToggleFreeCam(aToggleFreeCam),
	myOffset(aOffset),
	myMouseRotationSpeed(120.0f),
	myPitch(0.0f),
	myYaw(0.0f),
	myLimitFirstPerson(false)
{
}

CCameraControllerComponent::~CCameraControllerComponent()
{
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_FIRST_END_EVENT, this);
}

void CCameraControllerComponent::Awake()
{
	myCamera = CEngine::GetInstance()->GetActiveScene().MainCamera();
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_FIRST_END_EVENT, this);
}

void CCameraControllerComponent::Start()
{

}

void CCameraControllerComponent::Update()
{
#ifdef  _DEBUG
	// TEMPORARY
	if (Input::GetInstance()->IsKeyPressed(VK_F1))
	{
		if (myCameraMode != ECameraMode::UnlockCursor)
		{
			myPrevCameraMode = myCameraMode;
			myCameraMode = ECameraMode::UnlockCursor;
		}
		else
		{
			myCameraMode = myPrevCameraMode;
		}

		if (myCameraMode == ECameraMode::UnlockCursor)
			CEngine::GetInstance()->GetWindowHandler()->ShowAndUnlockCursor();
		else
			CEngine::GetInstance()->GetWindowHandler()->HideAndLockCursor();
	}
#endif

	// ! TEMPORARY ???
	if (!CEngine::GetInstance()->GetWindowHandler()->CursorLocked())
		return;

#ifdef  _DEBUG
	if (Input::GetInstance()->IsKeyPressed(/*std::toupper(myToggleFreeCam)*/myToggleFreeCam)) {
		myCameraMode = myCameraMode == ECameraMode::FreeCam ? ECameraMode::PlayerFirstPerson : ECameraMode::FreeCam;
		// So that the camera returns to the parent gameobject on return to ECameraMode::PlayerFirstPerson
		if (myCameraMode == ECameraMode::FreeCam)
			myPositionBeforeFreeCam = GameObject().myTransform->Position();
		else
			GameObject().myTransform->Position(myPositionBeforeFreeCam);
	}
#endif
	switch (myCameraMode)
	{
		case ECameraMode::MenuCam:
			break;

		case ECameraMode::FreeCam:
			UpdateFreeCam();
			break;

		case ECameraMode::PlayerFirstPerson:
			if(!myLimitFirstPerson)
				UpdatePlayerFirstPerson();
			break;

		case ECameraMode::UnlockCursor:
			break;

		default:break;
	}
}

CGameObject* CCameraControllerComponent::CreatePlayerFirstPersonCamera(CGameObject* aParentObject)
{
	CGameObject* camera = new CGameObject(PLAYER_CAMERA_ID);
	camera->AddComponent<CCameraComponent>(*camera, 70.0f);
	camera->AddComponent<CCameraControllerComponent>(*camera, 2.0f, ECameraMode::PlayerFirstPerson);
	camera->myTransform->SetParent(aParentObject->myTransform);
	camera->myTransform->Position({ 0.f,0.f,0.f });
	camera->myTransform->Rotation({ 0.f,0.f,0.f });
	return camera;
}

void CCameraControllerComponent::UpdatePlayerFirstPerson()
{
	Vector2 input = Input::GetInstance()->GetAxisRaw();
	RotateTransformWithYawAndPitch(input);
}

void CCameraControllerComponent::UpdateFreeCam()
{
	Vector2 input = Input::GetInstance()->GetAxisRaw();
	RotateTransformWithYawAndPitch(input);

	const float dt = CTimer::Dt();
	float verticalMoveSpeedModifier = 1.5f;
	DirectX::SimpleMath::Vector3 cameraMovementInput(0, 0, 0);
	cameraMovementInput.z = Input::GetInstance()->IsKeyDown('W') ?	myCameraMoveSpeed : cameraMovementInput.z;
	cameraMovementInput.z = Input::GetInstance()->IsKeyDown('S') ? -myCameraMoveSpeed : cameraMovementInput.z;
	cameraMovementInput.x = Input::GetInstance()->IsKeyDown('D') ?	myCameraMoveSpeed : cameraMovementInput.x;
	cameraMovementInput.x = Input::GetInstance()->IsKeyDown('A') ? -myCameraMoveSpeed : cameraMovementInput.x;
	cameraMovementInput.y = Input::GetInstance()->IsKeyDown('E') ?	myCameraMoveSpeed * verticalMoveSpeedModifier : cameraMovementInput.y;
	cameraMovementInput.y = Input::GetInstance()->IsKeyDown('Q') ? -myCameraMoveSpeed * verticalMoveSpeedModifier : cameraMovementInput.y;
	GameObject().myTransform->MoveLocal(cameraMovementInput * myCameraMoveSpeed * dt);
}

void CCameraControllerComponent::RotateTransformWithYawAndPitch(const Vector2& someInput)
{
	float sensitivity = 0.25f; //TestV�rde, K�ndes  okej p� min Dator! Bra � testa p� andras datorer! /Axel Savage 2021-04-09 14:00
	myYaw = WrapAngle(myYaw + (someInput.x * sensitivity));
	myPitch = std::clamp(myPitch + (someInput.y * sensitivity), ToDegrees(-PI / 2.0f)+0.1f, ToDegrees(PI / 2.0f)-0.1f);
	if (GameObject().myTransform->GetParent()) {
		GameObject().myTransform->Rotation({ myPitch, 0, 0 });
		GameObject().myTransform->GetParent()->Rotation({ 0, myYaw, 0 });
	}
	else {
		GameObject().myTransform->Rotation({ myPitch, myYaw, 0 });
	}
}

void CCameraControllerComponent::Receive(const SStringMessage& aMsg)
{
	if (PostMaster::CompareStringMessage(PostMaster::SMSG_FIRST_END_EVENT, aMsg.myMessageType))
	{
		myLimitFirstPerson = true;
		return;
	}
}

void CCameraControllerComponent::SetCameraMoveSpeed(float aCameraMoveSpeed) {
	myCameraMoveSpeed = aCameraMoveSpeed;
}



float CCameraControllerComponent::GetCameraMoveSpeed() {
	return myCameraMoveSpeed;
}
