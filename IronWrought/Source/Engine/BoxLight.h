#pragma once

class CDirectXFramework;

class CBoxLight
{
public:
	CBoxLight();
	~CBoxLight();

	bool Init();

	const Vector3& GetPosition() const;
	const Vector3& GetColor() const;
	const Vector3& GetDirection() const;
	//const Vector2& GetWidthAndHeight() const;
	const float GetIntensity() const;
	const float GetRange() const;

	const Matrix& GetWorldMatrix() const;
	const Matrix& GetViewMatrix() const;
	const Matrix& GetProjectionMatrix() const;

	void SetPosition(Vector3 aPosition);
	void SetColor(Vector3 aColor);
	void SetDirection(Vector3 aDirection);
	void SetRange(float aRange); // Range acts as depth of the light volume
	void SetIntensity(float anIntensity);
	void SetWidth(float aWidth);
	void SetHeight(float aHeight);
	void SetArea(Vector2 aWidthAndHeight);

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
	float myRange; // Range acts as depth of the light volume
	float myWidth;
	float myHeight;

	//DirectX::SimpleMath::Vector2 myShadowcastSize = { 32.0f, 32.0f };
	//DirectX::SimpleMath::Vector2 myShadowTextureSize = { 2048.0f * 4.0f, 2048.0f * 4.0f };
};