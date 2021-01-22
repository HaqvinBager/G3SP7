#include "stdafx.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "Scene.h"
#include "AStar.h"
#include "AnimationComponent.h"

#define ENGINE_SCALE 0.01f
using namespace DirectX::SimpleMath;

CTransformComponent::CTransformComponent(CGameObject& aParent, DirectX::SimpleMath::Vector3 aPosition)
	:  myScale(1.0f), 
	myParent(nullptr),
	CComponent(aParent)
{
	Scale(1.0f);
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
	if (myParent != nullptr)
	{
		myWorldRotation = myParent->myWorldRotation * myLocalRotation;
		myWorldScale = myParent->myWorldScale * myLocalScale;
		myWorldPosition = myParent->myWorldPosition + myParent->myWorldRotation * (myParent->myWorldScale * myLocalPosition);

		Matrix t, r, s;		
		// cache local/world transforms
		t = Matrix::CreateTranslation(myLocalPosition);
		r = Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(myLocalRotation.y),
										   DirectX::XMConvertToRadians(myLocalRotation.x),
										   DirectX::XMConvertToRadians(myLocalRotation.z));
		s = Matrix::CreateScale(myLocalScale);
		myLocalTransform = t * r * s;


		t = Matrix::CreateTranslation(myWorldPosition);
		r = Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(myWorldRotation.y),
										   DirectX::XMConvertToRadians(myWorldRotation.x),
										   DirectX::XMConvertToRadians(myWorldRotation.z));
		s = Matrix::CreateScale(myWorldScale);
		myTransform = t * r * s;
	}
}

void CTransformComponent::Position(DirectX::SimpleMath::Vector3 aPosition)
{
	if (myParent == nullptr)
	{
		myTransform.Translation(aPosition);
	}
	else
	{

		//myTransform.Translation()
	}

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
	myLocalRotation += aRotation;
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

void CTransformComponent::Scale(float aScale)
{
	myScale = aScale;
	myWorldScale = { aScale, aScale, aScale };
	myLocalScale = { aScale, aScale, aScale };
	ResetScale();
}

float CTransformComponent::Scale() const
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
	myWorldPosition += myTransform.Translation();

	if (Input::GetInstance()->IsKeyPressed('P'))
	{
		int a = 0;
		a += 0;
	}
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
