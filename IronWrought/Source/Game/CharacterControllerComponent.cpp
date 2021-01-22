#include "stdafx.h"
#include "CharacterControllerComponent.h"
#include "CharacterController.h"
#include "PhysXWrapper.h"

CCharacterControllerComponent::CCharacterControllerComponent(CGameObject& aParent, PxControllerShapeType::Enum aType, const Vector3& aPos, const float& aRadius, const float aHeight)
	: CBehaviour(aParent)
{
	myCharacterController = CMainSingleton::PhysXWrapper().CreateCharacterController(aType, aPos, aRadius, aHeight);
}

CCharacterControllerComponent::~CCharacterControllerComponent()
{
}

void CCharacterControllerComponent::Awake()
{
}

void CCharacterControllerComponent::Start()
{
}

void CCharacterControllerComponent::Update()
{
	PxVec3 moveInput(0, 0, 0);
	moveInput.z = Input::GetInstance()->IsKeyDown('W') ? 2.f : moveInput.z;
	moveInput.z = Input::GetInstance()->IsKeyDown('S') ? -2.f : moveInput.z;
	moveInput.x = Input::GetInstance()->IsKeyDown('D') ? 2.f : moveInput.x;
	moveInput.x = Input::GetInstance()->IsKeyDown('A') ? -2.f : moveInput.x;
	//moveInput.y = CMainSingleton::PhysXWrapper().GetPXScene()->getGravity().y;
	myCharacterController->GetController().move(moveInput, 1.f, CTimer::Dt(), NULL, NULL);
	std::cout << "X: " << moveInput.x << std::endl;
	std::cout << "Y: " << moveInput.y << std::endl;
	std::cout << "Z: " << moveInput.z << std::endl;
}

void CCharacterControllerComponent::OnEnable()
{
}

void CCharacterControllerComponent::OnDisable()
{
}
