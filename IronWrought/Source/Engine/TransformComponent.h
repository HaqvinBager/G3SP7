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

	void Position(DirectX::SimpleMath::Vector3 aPosition);
	DirectX::SimpleMath::Vector3 Position() const;

	void Rotation(DirectX::SimpleMath::Vector3 aRotation);
	void Rotation(DirectX::SimpleMath::Quaternion aQuaternion);
	DirectX::SimpleMath::Quaternion Rotation() const;

	void Scale(DirectX::SimpleMath::Vector3 aScale);
	DirectX::SimpleMath::Vector3 Scale() const;

	DirectX::SimpleMath::Matrix& Transform();
	DirectX::SimpleMath::Matrix GetMatrix() const;

	void Transform(DirectX::SimpleMath::Vector3 aPosition, DirectX::SimpleMath::Vector3 aRotation);

	void Move(DirectX::SimpleMath::Vector3 aMovement);
	void MoveLocal(DirectX::SimpleMath::Vector3 aMovement);
	void Rotate(DirectX::SimpleMath::Vector3 aRotation);
	void Rotate(DirectX::SimpleMath::Quaternion aQuaternion);

	void ResetScale();
	//To be Removed 2020-01-12 -> 2020-01-22 /Axel
	void SetOutlineScale();

private:
	DirectX::SimpleMath::Vector3 myScale;
	// TODO: Move speed into "Resource Component"
	DirectX::SimpleMath::Matrix myTransform;
	std::string hej;
};
