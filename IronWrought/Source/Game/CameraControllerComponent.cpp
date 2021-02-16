#include "stdafx.h"
#include "CameraControllerComponent.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "Scene.h"
#include <algorithm>

#define PI 3.14159265f

CCameraControllerComponent::CCameraControllerComponent(CGameObject& aGameObject, float aCameraMoveSpeed, ECameraMode aCameraMode, char aToggleFreeCam, Vector3 aOffset)
	: CComponent(aGameObject),
	myCameraMoveSpeed(aCameraMoveSpeed),
	myCamera(nullptr),
	myCameraMode(aCameraMode),
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
		bool showCursor = CEngine::GetInstance()->GetWindowHandler()->CursorLocked();
		CEngine::GetInstance()->GetWindowHandler()->LockCursor(!showCursor);
		myCameraMode = (myCameraMode == ECameraMode::UnlockCursor) ? ECameraMode::FreeCam : ECameraMode::UnlockCursor;
	}
	// ! TEMPORARY

	if (Input::GetInstance()->IsKeyPressed(/*std::toupper(myToggleFreeCam)*/myToggleFreeCam)) {
		myCameraMode = myCameraMode == ECameraMode::FreeCam ? ECameraMode::PlayerFirstPerson : ECameraMode::FreeCam;

		// TEMPORARY
		//bool showCursor = CEngine::GetInstance()->GetWindowHandler()->CursorLocked();
		//CEngine::GetInstance()->GetWindowHandler()->LockCursor(!showCursor);
		// TEMPORARY
	}
#endif
	if (myCameraMode == ECameraMode::MenuCam) {

	} else if (myCameraMode == ECameraMode::FreeCam) {
		UpdateFreeCam();
	} else if(myCameraMode == ECameraMode::UnlockCursor){
	} else {
		UpdatePlayerFirstPerson();
	}

	// TEMP
	if (Input::GetInstance()->IsKeyPressed(VK_SPACE))
	{
		CEngine::GetInstance()->GetPhysx().Raycast(GameObject().myTransform->Position(), GameObject().myTransform->Transform().Forward(), 50000.0f);
	}
	// ! TEMP
}

CGameObject* CCameraControllerComponent::CreatePlayerFirstPersonCamera(CGameObject* aParentObject)
{
	CGameObject* camera = new CGameObject(1000);
	camera->AddComponent<CCameraComponent>(*camera, 70.0f);
	camera->AddComponent<CCameraControllerComponent>(*camera, 2.0f, ECameraMode::PlayerFirstPerson);
	camera->myTransform->SetParent(aParentObject->myTransform);
	camera->myTransform->Position({ 0.0f, 1.6f, -0.22f });
	camera->myTransform->Rotation({ 0.0f, 0.0f, 0.0f });
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

	myYaw	= WrapAngle(myYaw + (dx * myMouseRotationSpeed * dt));
	myPitch = std::clamp(myPitch + (dy * myMouseRotationSpeed * dt), ToDegrees(-PI / 2.0f), ToDegrees(PI / 2.0f));

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
