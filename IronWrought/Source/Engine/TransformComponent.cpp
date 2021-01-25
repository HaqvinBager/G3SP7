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
	//if (myParent != nullptr)
	//{
	//	myWorldRotation = myParent->myWorldRotation * myLocalRotation;
	//	myWorldScale = myParent->myWorldScale * myLocalScale;
	//	myWorldPosition = myParent->myWorldPosition + myParent->myWorldRotation * (myParent->myWorldScale * myLocalPosition);

	//	Matrix t, r, s;		
	//	// cache local/world transforms
	//	t = Matrix::CreateTranslation(myLocalPosition);
	//	r = Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(myLocalRotation.y),
	//									   DirectX::XMConvertToRadians(myLocalRotation.x),
	//									   DirectX::XMConvertToRadians(myLocalRotation.z));
	//	s = Matrix::CreateScale(myLocalScale);
	//	myLocalTransform = t * r * s;


	//	t = Matrix::CreateTranslation(myWorldPosition);
	//	r = Matrix::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(myWorldRotation.y),
	//									   DirectX::XMConvertToRadians(myWorldRotation.x),
	//									   DirectX::XMConvertToRadians(myWorldRotation.z));
	//	s = Matrix::CreateScale(myWorldScale);
	//	myTransform = t * r * s;
	//}

	
}

void CTransformComponent::LateUpdate()
{
	if (myParent != nullptr)
	{



		
	/*	Vector3 thisChildOffset = myLocalTransform.Translation();
		Matrix childMatrix = myLocalTransform;

		Matrix tempRotation = Matrix::CreateFromQuaternion(myParent->Rotation());
		childMatrix = tempRotation;
		childMatrix *= Matrix::CreateScale(myScale * ENGINE_SCALE);
		childMatrix.Translation(thisChildOffset);
		myLocalTransform = childMatrix;*/
		
		//myLocalTransform.Translation(myLocalTransform.Translation() - myLocalTransform.Forward() * aMovement.z);

		Rotation(myParent->Rotation());

		Vector3 parentForward = myParent->myWorldTransform.Forward();
		parentForward.Normalize();

		Vector3 test = myParentOffset * parentForward;
		myLocalTransform.Translation(myParent->myWorldTransform.Translation() + myParentOffset * parentForward);
		myWorldTransform = DirectX::XMMatrixMultiply(myParent->myWorldTransform, myLocalTransform);

		
	}
	else
	{
		myWorldTransform = myLocalTransform;
	}
}

void CTransformComponent::Position(DirectX::SimpleMath::Vector3 aPosition)
{
	myParentOffset = aPosition;
	myLocalTransform.Translation(aPosition);
}

DirectX::SimpleMath::Vector3 CTransformComponent::Position() const
{
	return myLocalTransform.Translation();
}

void CTransformComponent::Rotation(DirectX::SimpleMath::Vector3 aRotation)
{
	Vector3 tempTranslation = myLocalTransform.Translation();

	Matrix tempRotation = Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(aRotation.y),
		DirectX::XMConvertToRadians(aRotation.x),
		DirectX::XMConvertToRadians(aRotation.z)
	);

	myLocalTransform = tempRotation;
	myLocalTransform *= Matrix::CreateScale(myScale * ENGINE_SCALE);
	myLocalTransform.Translation(tempTranslation);
	//myLocalRotation += aRotation;
}

void CTransformComponent::Rotation(DirectX::SimpleMath::Quaternion aQuaternion) 
{
	Vector3 tempTranslation = myLocalTransform.Translation();

	Matrix tempRotation = Matrix::CreateFromQuaternion(
		aQuaternion
	);
	myLocalTransform = tempRotation;
	myLocalTransform *= Matrix::CreateScale(myScale * ENGINE_SCALE);
	myLocalTransform.Translation(tempTranslation);
}

DirectX::SimpleMath::Quaternion CTransformComponent::Rotation() const
{
	DirectX::SimpleMath::Vector3 translation;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	GetLocalMatrix().Decompose(scale, quat, translation);
	return quat;
}

void CTransformComponent::Scale(float aScale)
{
	myScale = aScale;
	//myWorldScale = { aScale, aScale, aScale };
	//myLocalScale = { aScale, aScale, aScale };
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
	myLocalTransform.Decompose(scale, rotation, translation);
	myLocalTransform = Matrix::CreateFromQuaternion(rotation);
	myLocalTransform *= Matrix::CreateScale((myScale * ENGINE_SCALE) * 1.02f);
	myLocalTransform.Translation(translation);
}

void CTransformComponent::ResetScale()
{
	Vector3 scale;
	Vector3 translation;
	Quaternion rotation;
	myLocalTransform.Decompose(scale, rotation, translation);
	myLocalTransform = Matrix::CreateFromQuaternion(rotation);
	myLocalTransform *= Matrix::CreateScale(myScale * ENGINE_SCALE);
	myLocalTransform.Translation(translation);
}

void CTransformComponent::Transform(DirectX::SimpleMath::Vector3 aPosition, DirectX::SimpleMath::Vector3 aRotation)
{
	Rotation(aRotation);
	Position(aPosition);
}

DirectX::SimpleMath::Matrix& CTransformComponent::Transform()
{
	return myLocalTransform;
}

void CTransformComponent::Move(DirectX::SimpleMath::Vector3 aMovement)
{
	//Position(myLocalTransform.Translation() + aMovement);
	myLocalTransform.Translation(myLocalTransform.Translation() + aMovement);
}

void CTransformComponent::MoveLocal(DirectX::SimpleMath::Vector3 aMovement)
{
	//Position(myLocalTransform.Translation() + myLocalTransform.Right() * aMovement.x);
	//Position(myLocalTransform.Translation() + myLocalTransform.Up() * aMovement.y);
	//Position(myLocalTransform.Translation() - myLocalTransform.Forward() * aMovement.z);
	myLocalTransform.Translation(myLocalTransform.Translation() + myLocalTransform.Right() * aMovement.x);
	myLocalTransform.Translation(myLocalTransform.Translation() + myLocalTransform.Up() * aMovement.y);
	myLocalTransform.Translation(myLocalTransform.Translation() - myLocalTransform.Forward() * aMovement.z);
	//myWorldPosition += myTransform.Translation();
}

void CTransformComponent::Rotate(DirectX::SimpleMath::Vector3 aRotation)
{
	Vector3 tempTranslation = myLocalTransform.Translation();
	Matrix tempRotation = Matrix::CreateFromYawPitchRoll(aRotation.y, aRotation.x, aRotation.z);
	myLocalTransform *= tempRotation;
	myLocalTransform.Translation(tempTranslation);
}

void CTransformComponent::Rotate(DirectX::SimpleMath::Quaternion aQuaternion)
{
	Vector3 tempTranslation = myLocalTransform.Translation();
	Matrix tempRotation = Matrix::CreateFromQuaternion(aQuaternion);
	myLocalTransform *= tempRotation;
	myLocalTransform.Translation(tempTranslation);
}

DirectX::SimpleMath::Matrix CTransformComponent::GetWorldMatrix() const
{
	return myWorldTransform;
}

DirectX::SimpleMath::Matrix CTransformComponent::GetLocalMatrix() const
{
	return myLocalTransform;
}
