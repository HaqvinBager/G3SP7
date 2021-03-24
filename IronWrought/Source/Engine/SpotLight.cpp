#include "stdafx.h"
#include "SpotLight.h"

CSpotLight::CSpotLight()
    : myIntensity(1.0f)
    , myRange(1.0f)
    , myAngleExponent(1.0f)
{
}

CSpotLight::~CSpotLight()
{
}

bool CSpotLight::Init()
{
    myPosition = { 0.0f, 0.0f, 0.0f };
    myColor = { 1.0f, 1.0f, 1.0f };
    myDirection = { 0.0f, -1.0f, 0.0f };
   
    UpdateWorld();
    UpdateView();
    UpdateProjection();

    return true;
}

const Vector3& CSpotLight::GetPosition() const
{
    return myPosition;
}

const Vector3& CSpotLight::GetColor() const
{
    return myColor;
}

const Vector3& CSpotLight::GetDirection() const
{
    return myDirection;
}

const Vector4& CSpotLight::GetDirectionNormal1() const
{
    return myDirectionNormal1;
}

const Vector4& CSpotLight::GetDirectionNormal2() const
{
    return myDirectionNormal2;
}

const Vector4& CSpotLight::GetUpLeftCorner() const
{
    return myUpLeftCorner;
}

const Vector4& CSpotLight::GetUpRightCorner() const
{
    return myUpRightCorner;
}

const Vector4& CSpotLight::GetDownLeftCorner() const
{
    return myDownLeftCorner;
}

const Vector4& CSpotLight::GetDownRightCorner() const
{
    return myDownRightCorner;
}

const float CSpotLight::GetIntensity() const
{
    return myIntensity;
}

const float CSpotLight::GetRange() const
{
    return myRange;
}

const float CSpotLight::GetAngleExponent() const
{
    return myAngleExponent;
}

const Matrix& CSpotLight::GetWorldMatrix() const
{
    return myToWorldMatrix;
}

const Matrix& CSpotLight::GetViewMatrix() const
{
    return myToViewMatrix;
}

const Matrix& CSpotLight::GetProjectionMatrix() const
{
    return myToProjectionMatrix;
}

void CSpotLight::SetPosition(Vector3 aPosition)
{
    myPosition = aPosition;

    UpdateWorld();
    UpdateView();
}

void CSpotLight::SetColor(Vector3 aColor)
{
    myColor = aColor;
}

void CSpotLight::SetDirection(Vector3 aDirection)
{
    myDirection = aDirection;
    myDirection.Normalize();

    UpdateWorld();
    UpdateView();
}

void CSpotLight::SetRange(float aRange)
{
    myRange = aRange;

    UpdateProjection();
}

void CSpotLight::SetIntensity(float anIntensity)
{
    myIntensity = anIntensity;
}

void CSpotLight::SetWideness(float aWideness)
{
    myAngleExponent = 1.0f / aWideness;

    UpdateProjection();
}

void CSpotLight::UpdateWorld()
{
    const float s = std::sqrt(myDirection.x * myDirection.x + myDirection.y * myDirection.y + myDirection.z * myDirection.z);
    const float g = std::copysign(s, myDirection.z);  // note s instead of 1
    const float h = myDirection.z + g;
    Vector3 normal = Vector3(g * h - myDirection.x * myDirection.x, -myDirection.x * myDirection.y, -myDirection.x * h);
    normal.Normalize();
    Vector3 cross = myDirection.Cross(normal);
    cross.Normalize();

    myDirectionNormal1 = { normal.x, normal.y, normal.z, 0.0f };
    myDirectionNormal2 = { cross.x, cross.y, cross.z, 0.0f };

    Vector3 pos = { 0.0f, 0.0f, 0.0f };
    pos = myPosition + myDirection * myRange;
    myUpLeftCorner = { pos.x, pos.y, pos.z, 1.0f };
    myUpLeftCorner += -3.0f * myDirectionNormal1;
    myUpLeftCorner += 3.0f * myDirectionNormal2;
    myUpLeftCorner.w = 1.0f;

    myUpRightCorner = { pos.x, pos.y, pos.z, 1.0f };
    myUpRightCorner += 3.0f * myDirectionNormal1;
    myUpRightCorner += 3.0f * myDirectionNormal2;
    myUpRightCorner.w = 1.0f;

    myDownLeftCorner = { pos.x, pos.y, pos.z, 1.0f };
    myDownLeftCorner += -3.0f * myDirectionNormal1;
    myDownLeftCorner += -3.0f * myDirectionNormal2;
    myDownLeftCorner.w = 1.0f;

    myDownRightCorner = { pos.x, pos.y, pos.z, 1.0f };
    myDownRightCorner += 3.0f * myDirectionNormal1;
    myDownRightCorner += -3.0f * myDirectionNormal2;
    myDownRightCorner.w = 1.0f;

    myToWorldMatrix.Translation(myPosition);
}

void CSpotLight::UpdateView()
{
    myToViewMatrix = DirectX::XMMatrixLookAtLH(myPosition, myPosition - myDirection, Vector3::Up);
}

void CSpotLight::UpdateProjection()
{
    myToProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(myAngleExponent, 16.0f / 9.0f, 0.01f, myRange);
}