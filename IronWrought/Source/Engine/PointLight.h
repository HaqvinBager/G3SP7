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
	const SM::Vector3& GetPosition() const;
	const SM::Vector3& GetColor() const;
	float GetIntensity() const;
	float GetRange() const;

private:
	SM::Vector3 myPosition;
	SM::Vector3 myColor;
	float myIntensity;
	float myRange;
};

