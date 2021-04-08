#include "stdafx.h"
#include "BootUpState.h"

#include <SceneManager.h>
#include <Scene.h>

#include <SpriteInstance.h>
#include <SpriteFactory.h>

#include <CameraComponent.h>

#include <JsonReader.h>

#include <Input.h>

CBootUpState::CBootUpState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState)
	, myTimer(0.0f)
	, myLogoDisplayDuration(4.0f)
	, myFadeOutStart(3.0f)
	, myLogoToRender(0)
{

}

CBootUpState::~CBootUpState()
{
}

void CBootUpState::Awake()
{
	CScene* scene = CSceneManager::CreateEmpty();

	CEngine::GetInstance()->AddScene(myState, scene);
	CEngine::GetInstance()->SetActiveScene(myState);
}

void CBootUpState::Start()
{
	rapidjson::Document document = CJsonReader::Get()->LoadDocument("Json/Settings/SplashSettings.json");

	auto& scene = IRONWROUGHT->GetActiveScene();

	myLogos.emplace_back(new CSpriteInstance(scene, true));
	myLogos.back()->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["TGA Logo Path"].GetString())));
	myLogos.back()->SetShouldRender(true);

	myLogos.emplace_back(new CSpriteInstance(scene, true));
	myLogos.back()->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["Group Logo Path"].GetString())));
	myLogos.back()->SetShouldRender(false);

	CTimer::Mark();
	myTimer = 0.0f;
	myLogoToRender = 0;
}

void CBootUpState::Stop()
{
	myLogos.clear();
	CEngine::GetInstance()->RemoveScene(myState);
}

void CBootUpState::Update()
{
	myTimer += CTimer::Dt();

	for (auto& gameObject : CEngine::GetInstance()->GetActiveScene().myGameObjects)
	{
		gameObject->Update();
	}

	if (myTimer > myFadeOutStart)
	{
		auto color = myLogos[myLogoToRender]->GetColor();
		color.x = 1.0f - (myTimer - myFadeOutStart) / (myLogoDisplayDuration - myFadeOutStart);
		color.y = 1.0f - (myTimer - myFadeOutStart) / (myLogoDisplayDuration - myFadeOutStart);
		color.z = 1.0f - (myTimer - myFadeOutStart) / (myLogoDisplayDuration - myFadeOutStart);
		myLogos[myLogoToRender]->SetColor(color);
	}

	if (myTimer > myLogoDisplayDuration) {
		myTimer = 0.0f;

		myLogos[myLogoToRender]->SetShouldRender(false);
		myLogoToRender++;

		if (myLogoToRender >= myLogos.size())
		{
			myStateStack.PopTopAndPush(CStateStack::EState::MainMenu);
			return;
		}

		myLogos[myLogoToRender]->SetShouldRender(true);
	}

	if (INPUT->IsKeyPressed(VK_SPACE))
	{
		//myLogoToRender++;

		//if (myLogoToRender >= myLogos.size())
		//{
		//	myStateStack.PopTopAndPush(CStateStack::EState::MainMenu);
		//	return;
		//}
		myTimer = 0.0f;

		myLogos[myLogoToRender]->SetShouldRender(false);
		myLogoToRender++;

		if (myLogoToRender >= myLogos.size())
		{
			myStateStack.PopTopAndPush(CStateStack::EState::MainMenu);
			return;
		}

		myLogos[myLogoToRender]->SetShouldRender(true);
	}
}

void CBootUpState::Receive(const SStringMessage& /*aMessage*/)
{
}

void CBootUpState::Receive(const SMessage& /*aMessage*/)
{
}
