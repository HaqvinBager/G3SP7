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
	myPrevCameraMode(aCameraMode),
	myToggleFreeCam(aToggleFreeCam),
	myOffset(aOffset),
	myMouseRotationSpeed(120.0f),
	myPitch(0.0f),
	myYaw(0.0f),
	myOrbitRadius(10.0f)
{
}

CCameraControllerComponent::~CCameraControllerComponent()
{
}

void CCameraControllerComponent::Awake()
{
	myCamera = CEngine::GetInstance()->GetActiveScene().MainCamera();
	//CEngine::GetInstance()->GetWindowHandler()->HideAndLockCursor();
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

		case ECameraMode::OrbitCamera:
			UpdateOrbitCam();
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

inline constexpr float Lerp(const float& a, const float& b, const float& t)
{
	return ((a * t) + (b * (1.0f - t)));
}

void CCameraControllerComponent::UpdateOrbitCam()
{
	bool hideCursor = false;

	const float dt = CTimer::Dt();
	const float dy = static_cast<float>(Input::GetInstance()->MouseRawDeltaY());
	const float dx = static_cast<float>(Input::GetInstance()->MouseRawDeltaX());
	if (INPUT->IsMouseDown(Input::EMouseButton::Left) && GetAsyncKeyState(VK_LMENU))
	{
		hideCursor = true;
		const float rotationSpeed = 150.0f;
		myPhi	+= (dy * rotationSpeed * dt);
		myTheta += (dx * rotationSpeed * dt);
	}
	const float scroll = -static_cast<float>(INPUT->MouseWheel());
	const float zoomSpeed = 10.0f;
	const float newOrbitRadius = myOrbitRadius + (scroll * zoomSpeed * dt);
	myOrbitRadius = Lerp(myOrbitRadius, newOrbitRadius, 0.5f);// Attempt at smoothing scroll
	//myOrbitRadius += (scroll * zoomSpeed * dt);

	myPhi = std::clamp(myPhi, -89.0f, 89.0f);
	//theta = std::clamp(theta, -360.0f, 360.0f);// If rotation around equator should be limited.
	myOrbitRadius = std::clamp(myOrbitRadius, 0.1f, 80.0f);// Can't be zero:  EyePosition == FocusPosition is not allowed DirectX::XMMatrixLookAtLH(EyePosition, FocusPosition, UpDirection)

	float phiRadians = ToRadians(myPhi);
	float thetaRadians = ToRadians(myTheta);

	Vector3 viewPos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -myOrbitRadius, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(phiRadians, thetaRadians, 0.0f)
	);

	if (INPUT->IsMouseDown(Input::EMouseButton::Middle))
	{
		hideCursor = true;
		const float panningSpeed = 10.0f;

		const Matrix& cameraTransform = GameObject().myTransform->Transform();
		Matrix cameraRight = Matrix::CreateFromAxisAngle(cameraTransform.Right(), phiRadians);
		Matrix cameraUp = Matrix::CreateFromAxisAngle(cameraTransform.Up(), thetaRadians);

		myOrbitCenter = myOrbitCenter + Vector3(DirectX::XMVector3Transform({(-dx * panningSpeed * dt), 0.0f, 0.0f }, cameraUp));
		myOrbitCenter = myOrbitCenter + Vector3(DirectX::XMVector3Transform({0.0f,(dy * panningSpeed * dt), 0.0f }, cameraRight));
	}
	viewPos = viewPos + myOrbitCenter;
	Matrix view = DirectX::XMMatrixLookAtLH(
		viewPos, myOrbitCenter,
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);

	//view = view * DirectX::XMMatrixRotationRollPitchYaw(pitch, -yaw, roll);// Allows camera rotation around itself.
	view = view.Invert();
	GameObject().myTransform->Transform(view);

	if(hideCursor)
		CEngine::GetInstance()->GetWindowHandler()->HideAndLockCursor();
	else
		CEngine::GetInstance()->GetWindowHandler()->ShowAndUnlockCursor();
}

void CCameraControllerComponent::SetCameraMoveSpeed(float aCameraMoveSpeed) {
	myCameraMoveSpeed = aCameraMoveSpeed;
}

float CCameraControllerComponent::GetCameraMoveSpeed() {
	return myCameraMoveSpeed;
}
