#pragma once
#include "Component.h"

class CGameObject;
class CTransformComponent : public CComponent
{
public:
	CTransformComponent(CGameObject& aParent, DirectX::SimpleMath::Vector3 aPosition = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
	~CTransformComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	void Position(DirectX::SimpleMath::Vector3 aPosition);
	DirectX::SimpleMath::Vector3 Position() const;

	void Rotation(DirectX::SimpleMath::Vector3 aRotation);
	void Rotation(DirectX::SimpleMath::Quaternion aQuaternion);
	DirectX::SimpleMath::Quaternion Rotation() const;

	void Scale(float aScale);
	float Scale() const;

	DirectX::SimpleMath::Matrix& Transform();
	DirectX::SimpleMath::Matrix GetWorldMatrix() const;
	DirectX::SimpleMath::Matrix GetLocalMatrix() const;

	void Transform(DirectX::SimpleMath::Vector3 aPosition, DirectX::SimpleMath::Vector3 aRotation);

	void Move(DirectX::SimpleMath::Vector3 aMovement);
	void MoveLocal(DirectX::SimpleMath::Vector3 aMovement);
	void Rotate(DirectX::SimpleMath::Vector3 aRotation);
	void Rotate(DirectX::SimpleMath::Quaternion aQuaternion);

	void ResetScale();
	//To be Removed 2020-01-12 -> 2020-01-22 /Axel
	void SetOutlineScale();

	void SetParent(CTransformComponent* aParent) { myParent = aParent; }

private:
	// TODO: Move speed into "Resource Component"
	DirectX::SimpleMath::Matrix myLocalTransform;
	DirectX::SimpleMath::Matrix myWorldTransform;
	DirectX::SimpleMath::Vector3 myParentOffset;

	//DirectX::SimpleMath::Vector3 myLocalPosition;
	//DirectX::SimpleMath::Vector3 myLocalRotation;
	//DirectX::SimpleMath::Vector3 myLocalScale;

	//DirectX::SimpleMath::Vector3 myWorldPosition;
	//DirectX::SimpleMath::Vector3 myWorldRotation;
	//DirectX::SimpleMath::Vector3 myWorldScale;

	CTransformComponent* myParent;
	float myScale;
};
