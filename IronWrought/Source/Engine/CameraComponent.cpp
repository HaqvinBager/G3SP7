#include "stdafx.h"
#include "CameraComponent.h"
#include "RandomNumberGenerator.h"
#include "TransformComponent.h"
#include "SpriteInstance.h"
#include "SpriteFactory.h"

#include "Engine.h"
#include "Scene.h"
#include "PostMaster.h"
#include "MainSingleton.h"
#include "JsonReader.h"

CCameraComponent::CCameraComponent(CGameObject& aParent, const float aFoV/*, float aNearPlane, float aFarPlane, DirectX::SimpleMath::Vector2 aResolution*/)
	: CComponent(aParent),myFoV(aFoV)
{
	myProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(myFoV), (16.0f / 9.0f), 0.1f, 500.0f);
	myView = DirectX::XMMatrixLookAtLH(GameObject().myTransform->Position(), Vector3::Forward, Vector3::Up);
	
	DirectX::BoundingFrustum::CreateFromMatrix(myViewFrustum, myProjection);

	myTrauma = 0.0f;
	myShake = 0.0f;
	myDecayInSeconds = 0.5f;
	myShakeSpeed = 20.0f;
	myMaxShakeRotation = { 2.0f, 2.0f, 2.0f };
	myNoise = PerlinNoise(214125213);
	myShakeTimer = 0.0f;
	
	myFadingPlane = nullptr;
	myFadeTimer = 1.0f;
	myFadeSpeed = 2.75f;
	myFadingIn = true;
	myFadingPlaneActive = false;
}

CCameraComponent::~CCameraComponent()
{
	delete myFadingPlane;
	myFadingPlane = nullptr;
}

void CCameraComponent::Awake()
{
	rapidjson::Document document = CJsonReader::Get()->LoadDocument("Json/Settings/CameraInit.json");

	myFadingPlane = new CSpriteInstance();
	myFadingPlane->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["Fade Screen Path"].GetString())));
	myFadingPlane->SetSize({ 15.1f, 8.5f });
	myFadingPlane->SetRenderOrder(ERenderOrder::Layer3);
	myFadingPlane->SetShouldRender(false);
}

void CCameraComponent::Start()
{
}

float LogEaseOut(float x) {
	return 1 - (log(x) / log(0.01f));
}

float LogEaseIn(float x) {
	return (log(x) / log(0.01f));
}

void CCameraComponent::Update()
{
	if (Input::GetInstance()->IsKeyPressed('H'))
	{
		this->SetTrauma(1.5f);
	}

	if (myTrauma > 0.0f) {
		myShakeTimer += CTimer::Dt();
		myTrauma -= (1 / myDecayInSeconds) * CTimer::Dt();
		Shake();
	}

	if (myTrauma < 0.0f) {
		myTrauma = 0.0f;
	}

	if (myFadingPlaneActive) {
		
		if (!myFadingPlane->GetShouldRender())
			myFadingPlane->SetShouldRender(true);

		myFadeTimer -= myFadeSpeed * CTimer::Dt();
		if (myFadingIn) {
			DirectX::SimpleMath::Vector4 color = myFadingPlane->GetColor();
			float alpha = color.w;
			alpha = LogEaseOut(myFadeTimer);
			if (alpha <= 0.01f) {
				alpha = 0.0f;
				myFadingPlaneActive = false;
				myFadeTimer = 1.0f;
				myFadingPlane->SetShouldRender(false);
				CMainSingleton::PostMaster().SendLate({ EMessageType::FadeInComplete, 0 });
			}
			myFadingPlane->SetColor({ color.x, color.y, color.z, alpha });
		}
		else {
			DirectX::SimpleMath::Vector4 color = myFadingPlane->GetColor();
			float alpha = color.w;
			alpha = LogEaseIn(myFadeTimer);
			if (alpha >= 1.0f) {
				alpha = 1.0f;
				myFadingPlaneActive = false;
				myFadeTimer = 1.0f;
				myFadingPlane->SetShouldRender(false);
				CMainSingleton::PostMaster().SendLate({ EMessageType::FadeOutComplete, 0 });
			}
			myFadingPlane->SetColor({ color.x, color.y, color.z, alpha });
		}
	}
}

void CCameraComponent::SetTrauma(float aValue)
{
	myTrauma = aValue;
}

void CCameraComponent::SetStartingRotation(DirectX::SimpleMath::Vector3 aRotation)
{
	myStartingRotation = aRotation;
}

void CCameraComponent::SetFoV(float aFoV)
{
	myFoV = aFoV;
	myProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(myFoV), (16.0f / 9.0f), 0.1f, 500.0f);
	
	DirectX::BoundingFrustum::CreateFromMatrix(myViewFrustum, myProjection);
}

const DirectX::SimpleMath::Matrix& CCameraComponent::GetProjection() const
{
	return myProjection; 
}

float CCameraComponent::GetFoV()
{
	return myFoV;
}

void CCameraComponent::Fade(bool aShouldFadeIn)
{
	myFadingPlaneActive = true;
	myFadingIn = aShouldFadeIn;
}

const bool CCameraComponent::IsFading() const
{
	return myFadingPlaneActive;
}

void CCameraComponent::EmplaceSprites(std::vector<CSpriteInstance*>& someSprites) const
{
	if (myFadingPlaneActive)
	{
		someSprites.emplace_back(myFadingPlane);
	}
}

const Matrix& CCameraComponent::GetViewMatrix()
{
	myView = DirectX::XMMatrixLookAtLH(GameObject().myTransform->Position(), GameObject().myTransform->Position() - GameObject().myTransform->Transform().Forward(), GameObject().myTransform->Transform().Up());
	myView.Translation(Vector3::Zero);
	myView *= myShakenMatrix;
	myView.Translation(GameObject().myTransform->Transform().Translation());
	return myView;
}

const Matrix& CCameraComponent::GetShakenMatrix() const
{
	return myShakenMatrix;
}

const DirectX::BoundingFrustum CCameraComponent::GetViewFrustum()
{
	DirectX::BoundingFrustum viewFrustum;
	DirectX::XMVECTOR det;

	myViewFrustum.Transform(viewFrustum, DirectX::XMMatrixInverse(&det, DirectX::XMMatrixLookAtLH(GameObject().myTransform->Position(), GameObject().myTransform->Position() - GameObject().myTransform->Transform().Forward(), GameObject().myTransform->Transform().Up())));
	viewFrustum.Origin = GameObject().myTransform->WorldPosition();

	return viewFrustum;
}

void CCameraComponent::Shake()
{
	myShake = myTrauma * myTrauma * myTrauma;

	if (myShakeTimer > 0.0167f) {
		float newRotX = myMaxShakeRotation.x * myShake * (float(myNoise.noise(Random(-myShakeSpeed, myShakeSpeed), CTimer::Time() * myShakeSpeed, 0.0f) - 0.5f)) * 2;
		float newRotY = myMaxShakeRotation.y * myShake * (float(myNoise.noise(Random(-myShakeSpeed, myShakeSpeed), CTimer::Time() * myShakeSpeed, 0.0f) - 0.5f)) * 2;
		float newRotZ = myMaxShakeRotation.z * myShake * (float(myNoise.noise(Random(-myShakeSpeed, myShakeSpeed), CTimer::Time() * myShakeSpeed, 0.0f) - 0.5f)) * 2;

		DirectX::SimpleMath::Vector3 newRotation = { newRotX, newRotY, newRotZ };

		myShakenMatrix = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, DirectX::XMConvertToRadians(newRotZ));
		//myShakenMatrix = myShakenMatrix.Transpose();
		
		newRotation += myStartingRotation;
		//GameObject().myTransform->Rotation(newRotation);
		myShakeTimer -= 0.0167f;
	}
}


// FRUSTUM CULLING DEBUG

//myFrustumWireframe = new CLineInstance();
//DirectX::XMFLOAT3 corners[8];
//myViewFrustum.GetCorners(corners);
//std::vector<Vector3> cornerList;
//for (unsigned int i = 0; i < 8; ++i)
//{
//	cornerList.push_back(corners[i]);
//}
//
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[3]);
//
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[7]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[7]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[7]);
//
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[7]);
//
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[7]);
//
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[7]);
//
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[7]);
//
//myFrustumWireframe->Init(CLineFactory::GetInstance()->CreatePolygon(cornerList));

//	IRONWROUGHT->GetActiveScene().AddInstance(myFrustumWireframe);