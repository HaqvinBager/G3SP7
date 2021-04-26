#pragma once
//#include "..\DirectXTK\Inc\SimpleMath.h"

namespace SM = DirectX::SimpleMath;

class CPointLight {
public:
	CPointLight();
	~CPointLight();
	bool Init();
	void SetPosition(SM::Vector3 aDirection);
	void SetColor(SM::Vector3 aColor);
	void SetRange(float aRange);
	void SetIntensity(float aIntensity);

	void SetIsVolumetric(bool aShouldBeVolumetric);
	const bool& GetIsVolumetric() const;

	const Matrix& GetWorldMatrix() const;
	const SM::Vector3& GetPosition() const;
	const SM::Vector3& GetColor() const;
	float GetIntensity() const;
	float GetRange() const;

private:
	Matrix myToWorld;
	SM::Vector3 myPosition;
	SM::Vector3 myColor;
	float myIntensity;
	float myRange;
	bool myIsVolumetric;
};

