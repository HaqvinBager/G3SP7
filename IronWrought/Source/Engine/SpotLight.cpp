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

void CSpotLight::SetPosition(Vector3 aPosition)
{
    myPosition = aPosition;
}

void CSpotLight::SetColor(Vector3 aColor)
{
    myColor = aColor;
}

void CSpotLight::SetDirection(Vector3 aDirection)
{
    myDirection = aDirection;
}

void CSpotLight::SetRange(float aRange)
{
    myRange = aRange;
}

void CSpotLight::SetIntensity(float anIntensity)
{
    myIntensity = anIntensity;
}

void CSpotLight::SetWideness(float aWideness)
{
    myAngleExponent = 1.0f / aWideness;
}
