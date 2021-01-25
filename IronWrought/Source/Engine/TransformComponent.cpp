#include "stdafx.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "Scene.h"
#include "AStar.h"
#include "AnimationComponent.h"

//#define ENGINE_SCALE 0.01f
#define ENGINE_SCALE 1.0f

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

}

void CTransformComponent::LateUpdate()
{
	if (myParent != nullptr)
	{
		Matrix temp = myLocalTransform;
		//temp *= Matrix::CreateScale(myScale * 100.0f);
		myWorldTransform = DirectX::XMMatrixMultiply(temp, myParent->myWorldTransform);		
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
	if(myParent == nullptr)
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


void CTransformComponent::SetParent(CTransformComponent* aParent)
{


	static bool hasBeenParented = false;
	//if (myParent == nullptr) //We have a Prent
	//{
	//	if (aParent != nullptr)
	//	{
	//		//Setting a Parent for (previously without Parent)
	//		//Inverten av Parents Rotation - (this inverted rotation) 
	//		//Rotation(aParent->Rotation());
	//		//Matrix temp = myLocalTransform;
	//		//temp *= Matrix::CreateScale(myScale * 100.0f);
	//		//myWorldTransform = DirectX::XMMatrixMultiply(temp, aParent->myWorldTransform.Invert());
	//		/*Quaternion invParentRotation = aParent->Rotation();
	//		invParentRotation.Inverse(invParentRotation);
	//		Rotation(invParentRotation);*/

	//		//if (!hasBeenParented)
	//		//{
	//		//	Matrix newMatrix = DirectX::XMMatrixMultiply(myWorldTransform, aParent->myWorldTransform);
	//		//	//newMatrix *= Matrix::CreateScale(myScale * ENGINE_SCALE);
	//		//	myLocalTransform = newMatrix;
	//		//}
	//		//else
	//		//{
	//		//	Vector3 tempTranslation = myWorldTransform.Translation();
	//		//	Matrix tempRotation = Matrix::CreateFromQuaternion(
	//		//		Quaternion::Identity
	//		//	);
	//		//	myWorldTransform = tempRotation;
	//		//	//myWorldTransform *= Matrix::CreateScale(myScale * ENGINE_SCALE);
	//		//	myWorldTransform.Translation(tempTranslation);

	//		//	Matrix newMatrix = DirectX::XMMatrixMultiply(myWorldTransform, aParent->myWorldTransform);
	//		//	//newMatrix *= Matrix::CreateScale(myScale * 100.0f);
	//		//	myLocalTransform = newMatrix;
	//		//}


	//	}
	//}
	//else
	//{
		if (aParent == nullptr) //GOOOD
		{
			Vector3 positionOffset = Position();
			Quaternion rotationOffset = Rotation();

			//Removing Parent
			Matrix temp = myLocalTransform.Invert();
			temp.Translation({ 0, 0 ,0 });
			//temp *= Matrix::CreateScale(myScale * 100.0f);
			myLocalTransform = DirectX::XMMatrixMultiply(temp, myParent->myWorldTransform);
			Rotate(rotationOffset);
			Position(positionOffset);
			myWorldTransform = myLocalTransform;
		}
	//}

	myParent = aParent;
	if (myParent != nullptr)
		hasBeenParented = true;
}