#pragma once

class CSpotLight
{
public:
	CSpotLight();
	~CSpotLight();
	bool Init();

	const Vector3& GetPosition() const;
	const Vector3& GetColor() const;
	const Vector3& GetDirection() const;
	const float GetIntensity() const;
	const float GetRange() const;
	const float GetAngleExponent() const;

	void SetPosition(Vector3 aPosition);
	void SetColor(Vector3 aColor);
	void SetDirection(Vector3 aDirection);
	void SetRange(float aRange);
	void SetIntensity(float anIntensity);
	void SetWideness(float aWideness);

private:
	Vector3 myPosition;
	Vector3 myColor;
	Vector3 myDirection;
	float myIntensity;
	float myRange;
	float myAngleExponent; // Low values yield a wide cone, high values a narrow cone
};

