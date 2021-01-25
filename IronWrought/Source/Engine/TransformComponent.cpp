#include "stdafx.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "Scene.h"
#include "AStar.h"
#include "AnimationComponent.h"

#define ENGINE_SCALE 0.01f
using namespace DirectX::SimpleMath;

CTransformComponent::CTransformComponent(CGameObject& aParent, DirectX::SimpleMath::Vector3 aPosition): myScale(1.0f), CComponent(aParent)
{
	Scale({ 1.0f, 1.0f, 1.0f });
	Position(aPosition);
}

CTransformComponent::~CTransformComponent()
{
}

void CTransformComponent::Awake()
{
}

void CTransformComponent::Start()
{
	
}

void CTransformComponent::Update()
{
}

void CTransformComponent::Position(DirectX::SimpleMath::Vector3 aPosition)
{
	myTransform.Translation(aPosition);
}

DirectX::SimpleMath::Vector3 CTransformComponent::Position() const
{
	return myTransform.Translation();
}
void CTransformComponent::Rotation(DirectX::SimpleMath::Vector3 aRotation)
{
	Vector3 tempTranslation = myTransform.Translation();

	Matrix tempRotation = Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(aRotation.y),
		DirectX::XMConvertToRadians(aRotation.x),
		DirectX::XMConvertToRadians(aRotation.z)
	);
	myTransform = tempRotation;
	myTransform *= Matrix::CreateScale(myScale * ENGINE_SCALE);
	myTransform.Translation(tempTranslation);
}

void CTransformComponent::Rotation(DirectX::SimpleMath::Quaternion aQuaternion) 
{
	Vector3 tempTranslation = myTransform.Translation();

	Matrix tempRotation = Matrix::CreateFromQuaternion(
		aQuaternion
	);
	myTransform = tempRotation;
	myTransform *= Matrix::CreateScale(myScale * ENGINE_SCALE);
	myTransform.Translation(tempTranslation);
}

DirectX::SimpleMath::Quaternion CTransformComponent::Rotation() const
{
	DirectX::SimpleMath::Vector3 translation;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	GetMatrix().Decompose(scale, quat, translation);
	return quat;
}

void CTransformComponent::Scale(DirectX::SimpleMath::Vector3 aScale)
{
	myScale = aScale;
	ResetScale();
}

DirectX::SimpleMath::Vector3 CTransformComponent::Scale() const
{
	return myScale;
}

void CTransformComponent::SetOutlineScale()
{
	Vector3 scale;
	Vector3 translation;
	Quaternion rotation;
	myTransform.Decompose(scale, rotation, translation);
	myTransform = Matrix::CreateFromQuaternion(rotation);
	myTransform *= Matrix::CreateScale((myScale * ENGINE_SCALE) * 1.02f);
	myTransform.Translation(translation);
}

void CTransformComponent::ResetScale()
{
	Vector3 scale;
	Vector3 translation;
	Quaternion rotation;
	myTransform.Decompose(scale, rotation, translation);
	myTransform = Matrix::CreateFromQuaternion(rotation);
	myTransform *= Matrix::CreateScale(myScale * ENGINE_SCALE);
	myTransform.Translation(translation);
}

void CTransformComponent::Transform(DirectX::SimpleMath::Vector3 aPosition, DirectX::SimpleMath::Vector3 aRotation)
{
	Rotation(aRotation);
	Position(aPosition);
}

DirectX::SimpleMath::Matrix& CTransformComponent::Transform()
{
	return myTransform;
}

void CTransformComponent::Move(DirectX::SimpleMath::Vector3 aMovement)
{
	myTransform.Translation(myTransform.Translation() + aMovement);
}

void CTransformComponent::MoveLocal(DirectX::SimpleMath::Vector3 aMovement)
{
	myTransform.Translation(myTransform.Translation() + myTransform.Right() * aMovement.x);
	myTransform.Translation(myTransform.Translation() + myTransform.Up() * aMovement.y);
	myTransform.Translation(myTransform.Translation() - myTransform.Forward() * aMovement.z);
}

void CTransformComponent::Rotate(DirectX::SimpleMath::Vector3 aRotation)
{
	Vector3 tempTranslation = myTransform.Translation();
	Matrix tempRotation = Matrix::CreateFromYawPitchRoll(aRotation.y, aRotation.x, aRotation.z);
	myTransform *= tempRotation;
	myTransform.Translation(tempTranslation);
}

void CTransformComponent::Rotate(DirectX::SimpleMath::Quaternion aQuaternion)
{
	Vector3 tempTranslation = myTransform.Translation();
	Matrix tempRotation = Matrix::CreateFromQuaternion(aQuaternion);
	myTransform *= tempRotation;
	myTransform.Translation(tempTranslation);
}

DirectX::SimpleMath::Matrix CTransformComponent::GetMatrix() const
{
	return myTransform;
}
