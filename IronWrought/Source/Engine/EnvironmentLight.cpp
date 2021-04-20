#include "stdafx.h"
#include "EnvironmentLight.h"
#include "Engine.h"


CEnvironmentLight::CEnvironmentLight()
{
}

CEnvironmentLight::~CEnvironmentLight()
{
}

bool CEnvironmentLight::Init(CDirectXFramework* aFramework, std::string aFilePath)
{
	wchar_t* cubeMapPathWide = new wchar_t[aFilePath.length() + 1];
	std::copy(aFilePath.begin(), aFilePath.end(), cubeMapPathWide);
	cubeMapPathWide[aFilePath.length()] = 0;

	ENGINE_HR_BOOL_MESSAGE(DirectX::CreateDDSTextureFromFile(aFramework->GetDevice(), cubeMapPathWide, nullptr, &myCubeShaderResourceView), "Cubemap Texture could not be created.");

	delete[] cubeMapPathWide;

	ID3D11Resource* cubeResource;
	myCubeShaderResourceView->GetResource(&cubeResource);
	ID3D11Texture2D* cubeTexture = reinterpret_cast<ID3D11Texture2D*>(cubeResource);
	D3D11_TEXTURE2D_DESC cubeDescription = { 0 };
	cubeTexture->GetDesc(&cubeDescription);

	ENGINE_BOOL_POPUP((cubeDescription.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE), "Cubemap texture could not be read as a cubemap! Is loaded as a Texture2D instead of TextureCube.")

	myDirection = { 0.0f, 1.0f, 0.0f, 0.0f };
	myShadowmapViewMatrix = DirectX::XMMatrixLookAtLH(myPosition, myPosition - myDirection, Vector3::Up);

	myShadowcastSize = /*{ 32.0f, 32.0f }*/{ 128.0f, 128.0f };
	myShadowTextureSize = { 2048.0f/* * 4.0f*/, 2048.0f/* * 4.0f*/ };

	myShadowmapProjectionMatrix = DirectX::XMMatrixOrthographicLH(myShadowcastSize.x, myShadowcastSize.y, -40.0f, 40.0f);

	myNumberOfSamples = 16.0f;
	myLightPower = 500000.0f;
	myScatteringProbability = 0.0001f;
	myHenyeyGreensteinGValue = 0.0f;

	myIsVolumetric = false;

	return true;
}

ID3D11ShaderResourceView* const* CEnvironmentLight::GetCubeMap()
{
	return &myCubeShaderResourceView;
}

DirectX::SimpleMath::Matrix CEnvironmentLight::GetShadowTransform() const
{
	DirectX::SimpleMath::Matrix matrix = DirectX::SimpleMath::Matrix();
	float radiansY = atan2(-myDirection.x, -myDirection.z);
	float l = sqrt(myDirection.z * myDirection.z + myDirection.x * myDirection.x);
	float radiansX = atan2(myDirection.y, l);
	matrix *= DirectX::SimpleMath::Matrix::CreateRotationY(radiansY);
	matrix *= DirectX::SimpleMath::Matrix::CreateFromAxisAngle(matrix.Right(), radiansX);

	return matrix;
}

void CEnvironmentLight::SetDirection(DirectX::SimpleMath::Vector3 aDirection)
{
	aDirection.Normalize();
	myDirection.x = aDirection.x;
	myDirection.y = aDirection.y;
	myDirection.z = aDirection.z;

	myShadowmapViewMatrix = DirectX::XMMatrixLookAtLH(myPosition, myPosition - myDirection, Vector3::Up);
}

void CEnvironmentLight::SetColor(DirectX::SimpleMath::Vector3 aColor)
{
	myColor.x = aColor.x;
	myColor.y = aColor.y;
	myColor.z = aColor.z;
}

void CEnvironmentLight::SetIntensity(float anIntensity)
{
	myColor.w = anIntensity;
}

void CEnvironmentLight::SetPosition(DirectX::SimpleMath::Vector3 aPosition)
{
	myPosition = Vector4(aPosition.x, aPosition.y, aPosition.z, 1.0f);

	// Round to pixel positions
	Vector3 position = DirectX::SimpleMath::Vector3(myPosition);
	Vector2 unitsPerPixel = myShadowcastSize / myShadowTextureSize;
	Matrix shadowTransform = GetShadowTransform();

	float rightStep = position.Dot(shadowTransform.Right());
	position -= rightStep * shadowTransform.Right();
	rightStep = floor(rightStep / unitsPerPixel.x) * unitsPerPixel.x;
	position += rightStep * shadowTransform.Right();

	float upStep = position.Dot(shadowTransform.Up());
	position -= upStep * shadowTransform.Up();
	upStep = floor(upStep / unitsPerPixel.y) * unitsPerPixel.y;
	position += upStep * shadowTransform.Up();

	myPosition = Vector4(position.x, position.y, position.z, 1.0f);

	myShadowmapViewMatrix = DirectX::XMMatrixLookAtLH(myPosition, myPosition - myDirection, Vector3::Up);
}

void CEnvironmentLight::SetNumberOfSamples(float aNumberOfSamples)
{
	myNumberOfSamples = aNumberOfSamples;
}

void CEnvironmentLight::SetLightPower(float aPower)
{
	myLightPower = aPower;
}

void CEnvironmentLight::SetScatteringProbability(float aScatteringProbability)
{
	myScatteringProbability = aScatteringProbability;
}

void CEnvironmentLight::SetHenyeyGreensteinGValue(float aGValue)
{
	myHenyeyGreensteinGValue = aGValue;
}

void CEnvironmentLight::SetIsVolumetric(bool aShouldBeVolumetric)
{
	myIsVolumetric = aShouldBeVolumetric;
}

DirectX::SimpleMath::Matrix CEnvironmentLight::GetShadowView() const
{
	return myShadowmapViewMatrix;
	// For fog
	//return DirectX::XMMatrixLookAtLH(GetShadowPosition(), myDirection, Vector3::Up);
}

DirectX::SimpleMath::Matrix CEnvironmentLight::GetShadowProjection() const
{
	return myShadowmapProjectionMatrix;
}

DirectX::SimpleMath::Vector4 CEnvironmentLight::GetShadowPosition() const
{
	return myPosition;/*
	DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(myPosition);
	DirectX::SimpleMath::Vector2 unitsPerPixel = myShadowcastSize / myShadowTextureSize;
	DirectX::SimpleMath::Matrix shadowTransform = GetShadowTransform();

	float rightStep = position.Dot(shadowTransform.Right());
	position -= rightStep * shadowTransform.Right();
	rightStep = floor(rightStep / unitsPerPixel.x) * unitsPerPixel.x;
	position += rightStep * shadowTransform.Right();

	float upStep = position.Dot(shadowTransform.Up());
	position -= upStep * shadowTransform.Up();
	upStep = floor(upStep / unitsPerPixel.y) * unitsPerPixel.y;
	position += upStep * shadowTransform.Up();

	return DirectX::SimpleMath::Vector4(position.x, position.y, position.z, 1.0f);*/
}

const float& CEnvironmentLight::GetNumberOfSamples() const
{
	return myNumberOfSamples;
}

const float& CEnvironmentLight::GetLightPower() const
{
	return myLightPower;
}

const float& CEnvironmentLight::GetScatteringProbability() const
{
	return myScatteringProbability;
}

const float& CEnvironmentLight::GetHenyeyGreensteinGValue() const
{
	return myHenyeyGreensteinGValue;
}

const bool& CEnvironmentLight::GetIsVolumetric() const
{
	return myIsVolumetric;
}
