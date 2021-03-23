#include "stdafx.h"
#include "CameraControllerComponent.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "Scene.h"
#include <algorithm>
#include "PlayerControllerComponent.h"
#include "CharacterController.h"

#define PI 3.14159265f

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
	myYaw(0.0f)
{
}

CCameraControllerComponent::~CCameraControllerComponent()
{
}

void CCameraControllerComponent::Awake()
{
	myCamera = CEngine::GetInstance()->GetActiveScene().MainCamera();
	CEngine::GetInstance()->GetWindowHandler()->HideAndLockCursor();
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
	// ! TEMPORARY

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
			UpdatePlayerFirstPerson();
			break;

		case ECameraMode::UnlockCursor:
			break;

		default:break;
	}
}

CGameObject* CCameraControllerComponent::CreatePlayerFirstPersonCamera(CGameObject* aParentObject)
{
	CGameObject* camera = new CGameObject(1000);
	camera->AddComponent<CCameraComponent>(*camera, 70.0f);
	camera->AddComponent<CCameraControllerComponent>(*camera, 2.0f, ECameraMode::PlayerFirstPerson);
	camera->myTransform->SetParent(aParentObject->myTransform);
	camera->myTransform->Position({ 0.f,0.f,0.f });
	camera->myTransform->Rotation({ 0.f,0.f,0.f });
	return camera;
}

float WrapAngle(float anAngle)
{
	return fmodf(anAngle, 360.0f);
}

float ToDegrees(float anAngleInRadians)
{
	return anAngleInRadians * (180.0f / PI);
}

float ToRadians(float anAngleInDegrees)
{
	return anAngleInDegrees * (PI / 180.0f);
}

void CCameraControllerComponent::UpdatePlayerFirstPerson()
{
	const float dt = CTimer::Dt();
	float dx = static_cast<float>(Input::GetInstance()->MouseRawDeltaX());
	float dy = static_cast<float>(Input::GetInstance()->MouseRawDeltaY());

	float myOldYaw = myYaw;
	float myOldPitch = myPitch;

	myYaw	= WrapAngle(myYaw + (dx * myMouseRotationSpeed * dt));
	myPitch = std::clamp(myPitch + (dy * myMouseRotationSpeed * dt), ToDegrees(-PI / 2.0f), ToDegrees(PI / 2.0f));

	myYaw = Lerp(myOldYaw, myYaw, 0.6f);
	myPitch = Lerp(myOldPitch, myPitch, 0.6f);

	GameObject().myTransform->Rotation({ myPitch, myYaw, 0});

	if (CEngine::GetInstance()->GetWindowHandler()->CursorLocked()) {
		auto screenDimensions = CEngine::GetInstance()->GetWindowHandler()->GetResolution();
		Input::GetInstance()->SetMouseScreenPosition(static_cast<int>(screenDimensions.x / 2.0f), static_cast<int>(screenDimensions.y / 2.0f));
	}
}

void CCameraControllerComponent::UpdateFreeCam()
{
	const float dt = CTimer::Dt();
	float verticalMoveSpeedModifier = 1.5f;
	DirectX::SimpleMath::Vector3 cameraMovementInput(0, 0, 0);
	cameraMovementInput.z = Input::GetInstance()->IsKeyDown('W') ?	myCameraMoveSpeed : cameraMovementInput.z;
	cameraMovementInput.z = Input::GetInstance()->IsKeyDown('S') ? -myCameraMoveSpeed : cameraMovementInput.z;
	cameraMovementInput.x = Input::GetInstance()->IsKeyDown('D') ?	myCameraMoveSpeed : cameraMovementInput.x;
	cameraMovementInput.x = Input::GetInstance()->IsKeyDown('A') ? -myCameraMoveSpeed : cameraMovementInput.x;
	cameraMovementInput.y = Input::GetInstance()->IsKeyDown('E') ?	myCameraMoveSpeed * verticalMoveSpeedModifier : cameraMovementInput.y;
	cameraMovementInput.y = Input::GetInstance()->IsKeyDown('Q') ? -myCameraMoveSpeed * verticalMoveSpeedModifier : cameraMovementInput.y;

	float dx = static_cast<float>(Input::GetInstance()->MouseRawDeltaX());
	float dy = static_cast<float>(Input::GetInstance()->MouseRawDeltaY());

	myYaw = WrapAngle(myYaw + (dx * myMouseRotationSpeed * dt));
	myPitch = std::clamp(myPitch + (dy * myMouseRotationSpeed * dt), ToDegrees(-PI / 2.0f), ToDegrees(PI / 2.0f));

	GameObject().myTransform->MoveLocal(cameraMovementInput * myCameraMoveSpeed * dt);
	GameObject().myTransform->Rotation({ myPitch, myYaw, 0});

	if (CEngine::GetInstance()->GetWindowHandler()->CursorLocked()) {
		auto screenDimensions = CEngine::GetInstance()->GetWindowHandler()->GetResolution();
		Input::GetInstance()->SetMouseScreenPosition(static_cast<int>(screenDimensions.x / 2.0f), static_cast<int>(screenDimensions.y / 2.0f));
	}
}
void CCameraControllerComponent::SetCameraMoveSpeed(float aCameraMoveSpeed) {
	myCameraMoveSpeed = aCameraMoveSpeed;
}

float CCameraControllerComponent::GetCameraMoveSpeed() {
	return myCameraMoveSpeed;
}
