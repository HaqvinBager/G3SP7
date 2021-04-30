#include "stdafx.h"
#include "BoxLight.h"

CBoxLight::CBoxLight()
    : myIntensity(1.0f)
    , myRange(1.0f)
    , myWidth(1.0f)
    , myHeight(1.0f)
{
}

CBoxLight::~CBoxLight()
{
}

bool CBoxLight::Init()
{
    myPosition = { 0.0f, 0.0f, 0.0f };
    myColor = { 1.0f, 1.0f, 1.0f };
    myDirection = { 0.0f, 0.0f, 1.0f };

    UpdateWorld();
    UpdateView();
    UpdateProjection();

    return true;
}

const Vector3& CBoxLight::GetPosition() const
{
    return myPosition;
}

const Vector3& CBoxLight::GetColor() const
{
    return myColor;
}

const Vector3& CBoxLight::GetDirection() const
{
    return myDirection;
}

const Vector4& CBoxLight::GetDirectionNormal1() const
{
    return myDirectionNormal1;
}

const Vector4& CBoxLight::GetDirectionNormal2() const
{
    return myDirectionNormal2;
}

const float CBoxLight::GetWidth() const
{
    return myWidth;
}

const float CBoxLight::GetHeight() const
{
    return myHeight;
}

const float CBoxLight::GetIntensity() const
{
    return myIntensity;
}

const float CBoxLight::GetRange() const
{
    return myRange;
}

const Matrix& CBoxLight::GetWorldMatrix() const
{
    return myToWorldMatrix;
}

const Matrix& CBoxLight::GetViewMatrix() const
{
    return myToViewMatrix;
}

const Matrix& CBoxLight::GetProjectionMatrix() const
{
    return myToProjectionMatrix;
}

void CBoxLight::SetPosition(Vector3 aPosition)
{
    myPosition = aPosition;

    UpdateWorld();
    UpdateView();
}

void CBoxLight::SetColor(Vector3 aColor)
{
    myColor = aColor;
}

void CBoxLight::SetDirection(Vector3 aDirection)
{
    myDirection = aDirection;

    UpdateWorld();
    UpdateView();
}

void CBoxLight::SetRange(float aRange)
{
    myRange = aRange;

    UpdateWorld();
    UpdateProjection();
}

void CBoxLight::SetIntensity(float anIntensity)
{
    myIntensity = anIntensity;
}

void CBoxLight::SetWidth(float aWidth)
{
    myWidth = aWidth;

    UpdateWorld();
    UpdateProjection();
}

void CBoxLight::SetHeight(float aHeight)
{
    myHeight = aHeight;

    UpdateWorld();
    UpdateProjection();
}

void CBoxLight::SetArea(Vector2 aWidthAndHeight)
{
    myWidth = aWidthAndHeight.x;
    myHeight = aWidthAndHeight.y;

    UpdateWorld();
    UpdateProjection();
}

void CBoxLight::SetRotation(Vector3 aRotation)
{
    myEulerAngles = aRotation;

    //Vector3 tempTranslation = myToWorldMatrix.Translation();

    Matrix tempRotation = Matrix::CreateFromYawPitchRoll(
        DirectX::XMConvertToRadians(aRotation.y),
        DirectX::XMConvertToRadians(aRotation.x),
        DirectX::XMConvertToRadians(aRotation.z)
    );

    myToWorldMatrix = Matrix::CreateScale({myWidth, myHeight, myRange});
    myToWorldMatrix *= tempRotation;
    myToWorldMatrix.Translation(myPosition);

    myDirection = -myToWorldMatrix.Forward();
    myDirectionNormal1 = { myToWorldMatrix.Right().x, myToWorldMatrix.Right().y, myToWorldMatrix.Right().z, 0.0f };
    myDirectionNormal2 = { myToWorldMatrix.Up().x, myToWorldMatrix.Up().y, myToWorldMatrix.Up().z, 0.0f };

    myDirection.Normalize();
    myDirectionNormal1.Normalize();
    myDirectionNormal2.Normalize();

    UpdateView();
}

void CBoxLight::Rotate(Vector3 aRotation)
{
    myEulerAngles += aRotation;

    myEulerAngles.x = fmodf(myEulerAngles.x, 360.0f);
    myEulerAngles.y = fmodf(myEulerAngles.y, 360.0f);
    myEulerAngles.z = fmodf(myEulerAngles.z, 360.0f);

    if (myEulerAngles.x < 0.0f)
        myEulerAngles.x += 360.0f;
    if (myEulerAngles.y < 0.0f)
        myEulerAngles.y += 360.0f;
    if (myEulerAngles.z < 0.0f)
        myEulerAngles.z += 360.0f;

    //Vector3 tempTranslation = myToWorldMatrix.Translation();
    //Matrix tempRotation = Matrix::CreateFromYawPitchRoll(aRotation.y, aRotation.x, aRotation.z);
    Matrix tempRotation = Matrix::CreateFromYawPitchRoll(
        DirectX::XMConvertToRadians(aRotation.y),
        DirectX::XMConvertToRadians(aRotation.x),
        DirectX::XMConvertToRadians(aRotation.z)
    );
    myToWorldMatrix *= tempRotation;
    myToWorldMatrix.Translation(myPosition);

    myDirection = -myToWorldMatrix.Forward();
    myDirectionNormal1 = { myToWorldMatrix.Right().x, myToWorldMatrix.Right().y, myToWorldMatrix.Right().z, 0.0f };
    myDirectionNormal2 = { myToWorldMatrix.Up().x, myToWorldMatrix.Up().y, myToWorldMatrix.Up().z, 0.0f };

    myDirection.Normalize();
    myDirectionNormal1.Normalize();
    myDirectionNormal2.Normalize();

    UpdateView();
}

void CBoxLight::UpdateWorld()
{
    //Matrix scale = Matrix::CreateScale(myWidth, myHeight, myRange);

    //const float s = std::sqrt(myDirection.x * myDirection.x + myDirection.y * myDirection.y + myDirection.z * myDirection.z);
    //const float g = std::copysign(s, myDirection.z);  // note s instead of 1
    //const float h = myDirection.z + g;
    //Vector3 normal = Vector3(g * h - myDirection.x * myDirection.x, -myDirection.x * myDirection.y, -myDirection.x * h);
    //normal.Normalize();
    //Vector3 cross = myDirection.Cross(normal);
    //cross.Normalize();

    //myToWorldMatrix._11 = normal.x;
    //myToWorldMatrix._21 = normal.y;
    //myToWorldMatrix._31 = normal.z;

    //myToWorldMatrix._12 = cross.x;
    //myToWorldMatrix._22 = cross.y;
    //myToWorldMatrix._32 = cross.z;

    //myToWorldMatrix._13 = myDirection.x;
    //myToWorldMatrix._23 = myDirection.y;
    //myToWorldMatrix._33 = myDirection.z;

    SetRotation(myEulerAngles);

    //myToWorldMatrix.Translation({ 0.0f, 0.0f, 0.0f });
    //myToWorldMatrix *= scale;

    //myToWorldMatrix.Translation(myPosition);

    //myDirectionNormal1 = { normal.x, normal.y, normal.z, 0.0f };
    //myDirectionNormal2 = { cross.x, cross.y, cross.z, 0.0f };


    //Vector3 z = myDirection;
    //Vector3 x = Vector3::Up.Cross(z);
    //x.Normalize();
    //Vector3 y = 
    //    xaxis = normal(cross(Up, zaxis))
    //    yaxis = cross(zaxis, xaxis)

    //    xaxis.x           yaxis.x           zaxis.x          0
    //    xaxis.y           yaxis.y           zaxis.y          0
    //    xaxis.z           yaxis.z           zaxis.z          0
}

void CBoxLight::UpdateView()
{
    myToViewMatrix = DirectX::XMMatrixLookAtLH(myPosition, myPosition + myDirection, Vector3::Up);
}

void CBoxLight::UpdateProjection()
{
    myToProjectionMatrix = DirectX::XMMatrixOrthographicLH(myWidth, myHeight, 0.0f, myRange);
}
