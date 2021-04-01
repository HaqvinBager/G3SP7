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
	const Vector4& GetDirectionNormal1() const;
	const Vector4& GetDirectionNormal2() const;
	const Vector4& GetUpLeftCorner() const;
	const Vector4& GetUpRightCorner() const;
	const Vector4& GetDownLeftCorner() const;
	const Vector4& GetDownRightCorner() const;
	const float GetIntensity() const;
	const float GetRange() const;
	const float GetAngleExponent() const;

	const Matrix& GetWorldMatrix() const;
	const Matrix& GetViewMatrix() const;
	const Matrix& GetProjectionMatrix() const;

	void SetPosition(Vector3 aPosition);
	void SetColor(Vector3 aColor);
	void SetDirection(Vector3 aDirection);
	void SetRange(float aRange);
	void SetIntensity(float anIntensity);
	void SetWideness(float aWideness);

private:
	void UpdateWorld();
	void UpdateView();
	void UpdateProjection();

private:
	Matrix myToWorldMatrix;
	Matrix myToViewMatrix;
	Matrix myToProjectionMatrix;
	Vector4 myDirectionNormal1;
	Vector4 myDirectionNormal2;
	Vector4 myUpLeftCorner;
	Vector4 myUpRightCorner;
	Vector4 myDownLeftCorner;
	Vector4 myDownRightCorner;
	Vector3 myPosition;
	Vector3 myColor;
	Vector3 myDirection;
	float myIntensity;
	float myRange;
	float myAngleExponent; // Low values yield a wide cone, high values a narrow cone
};

