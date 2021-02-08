#include "stdafx.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "Scene.h"
#include "AStar.h"
#include "AnimationComponent.h"

//#define ENGINE_SCALE 0.01f
//#define ENGINE_SCALE 1.0f

using namespace DirectX::SimpleMath;

CTransformComponent::CTransformComponent(CGameObject& aParent, DirectX::SimpleMath::Vector3 aPosition)
	:  myScale(1.0f), 
	myParent(nullptr),
	CComponent(aParent)
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

void CTransformComponent::LateUpdate()
{
	if (myParent != nullptr)
	{
		Matrix temp = myLocalTransform;
		//temp *= Matrix::CreateScale(myScale * 100.0f);
		myWorldTransform = DirectX::XMMatrixMultiply(temp, myParent->myWorldTransform);		
		//DirectX::SimpleMath::Vector3 translation;
		//DirectX::SimpleMath::Vector3 scale;
		//DirectX::SimpleMath::Quaternion quat;
		//myWorldTransform.Decompose(scale, quat, translation);
		//std::cout << scale.x << std::endl;
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
	myLocalTransform *= Matrix::CreateScale(myScale);
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
	myLocalTransform *= Matrix::CreateScale(myScale);
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

void CTransformComponent::Scale(DirectX::SimpleMath::Vector3 aScale)
{
	myScale = aScale;
	//myWorldScale = { aScale, aScale, aScale };
	//myLocalScale = { aScale, aScale, aScale };
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
	myLocalTransform.Decompose(scale, rotation, translation);
	myLocalTransform = Matrix::CreateFromQuaternion(rotation);
	myLocalTransform *= Matrix::CreateScale(myScale * 1.02f);
	myLocalTransform.Translation(translation);
}

void CTransformComponent::ResetScale()
{
	Vector3 scale;
	Vector3 translation;
	Quaternion rotation;
	myLocalTransform.Decompose(scale, rotation, translation);
	myLocalTransform = Matrix::CreateFromQuaternion(rotation);
	if (myParent == nullptr) {
		myLocalTransform *= Matrix::CreateScale(myScale);
	}
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
	//DirectX::SimpleMath::Vector3 translation;
	//DirectX::SimpleMath::Vector3 scale;
	//DirectX::SimpleMath::Quaternion quat;
	//GetLocalMatrix().Decompose(scale, quat, translation);
	//
	//DirectX::SimpleMath::Vector3 parentTranslation;
	//DirectX::SimpleMath::Vector3 parentScale;
	//DirectX::SimpleMath::Quaternion parentQuat;
	//aParent->GetLocalMatrix().Decompose(parentScale, parentQuat, parentTranslation);
	////
	//std::cout << "Before: " << Position().x << std::endl;
	//Position(translation - parentTranslation);
	//std::cout << "After: " << Position().x << std::endl;

	//std::cout << "Rotation Before: " << Rotation().x << std::endl;
	//Rotation(quat - parentQuat);
	//std::cout << "Rotation Before: " << Rotation().x << std::endl;
	//Matrix temp = myLocalTransform;
	//temp *= Matrix::CreateScale(myScale * 100.0f);
	//myLocalTransform = DirectX::XMMatrixMultiply(temp, aParent->myWorldTransform);
	//std::cout << Position().x << std::endl;
	//std::cout << Position().z << std::endl;
	//myLocalTransform = DirectX::XMMatrixMultiply(temp, aParent->myWorldTransform);

	//scale /= 100.f;

	Matrix temp = myLocalTransform;
	//if (scale.x > 0.01f) {
	//	temp *= Matrix::CreateScale(myScale * 1/1000);
	//}
	//Scale(100);
	myLocalTransform = DirectX::XMMatrixMultiply(temp, aParent->myWorldTransform.Invert());
	//ResetScale();
	myParent = aParent;

	/*Vector3 translation;
	Vector3 scale;
	Quaternion quat;
	GetLocalMatrix().Decompose(scale, quat, translation);
	
	std::cout << "SCALE: " << scale.x << std::endl;*/
	//Scale(myScale);
}

void CTransformComponent::RemoveParent()
{

	//DirectX::SimpleMath::Vector3 parentTranslation;
	//DirectX::SimpleMath::Vector3 parentScale;
	//DirectX::SimpleMath::Quaternion parentQuat;
	//myParent->GetLocalMatrix().Decompose(parentScale, parentQuat, parentTranslation);

	//std::cout << "WorldPosition Before: " << myWorldTransform.Translation().x << std::endl;
	//std::cout << "Position Before: " << Position().x << std::endl;
	//Position(myWorldTransform.Translation());
	//std::cout << "Position After: " << Position().x << std::endl;

	////std::cout << "Rotation Before: " << Rotation().x << std::endl;
	//Rotation(quat);
	////std::cout << "Rotation Before: " << Rotation().x << std::endl;
	//ResetScale();
	//myLocalTransform = myWorldTransform;

	Matrix temp = myLocalTransform;
	//temp *= Matrix::CreateScale(100);
	//myLocalTransform = DirectX::XMMatrixMultiply(myParent->myLocalTransform.Invert(), temp);
	myLocalTransform = DirectX::XMMatrixMultiply(temp, myParent->myWorldTransform);
	//Position(Position() / 100);
	//Scale(1/10);
	std::cout << "Position After X: " << Position().x << std::endl;
	std::cout << "Position After Y: " << Position().y << std::endl;
	std::cout << "Position After Z: " << Position().z << std::endl;
	Vector3 translation;
	Vector3 scale;
	Quaternion quat;
	GetLocalMatrix().Decompose(scale, quat, translation);

	std::cout << "SCALE: " << scale.x << std::endl;

	myParent = nullptr;
}
