#include "stdafx.h"
#include "Canvas.h"
#include "MainSingleton.h"
#include "PostMaster.h"
#include "Button.h"
#include "SpriteInstance.h"
#include "TextInstance.h"
#include "TextFactory.h"
#include "AnimatedUIElement.h"
#include "InputMapper.h"
#include "Input.h"
#include "SpriteFactory.h"
#include "Sprite.h"
#include "MainSingleton.h"
#include "Engine.h"
#include "Scene.h"

//#include "rapidjson\document.h"
//#include "rapidjson\istreamwrapper.h"

using namespace rapidjson;

CCanvas::CCanvas() :
	myBackground(nullptr),
	myIsEnabled(true)
{
}

CCanvas::~CCanvas()
{
	UnsubscribeToMessages();
	myMessageTypes.clear();
	delete myBackground;
	myBackground = nullptr;

	CScene& scene = IRONWROUGHT_ACTIVE_SCENE;

	for (size_t i = 0; i < myAnimatedUIs.size(); ++i)
	{
		scene.RemoveInstance(myAnimatedUIs[i]);
		delete myAnimatedUIs[i];
		myAnimatedUIs[i] = nullptr;
	}
	myAnimatedUIs.clear();

	for (size_t i = 0; i < myButtons.size(); ++i)
	{
			delete myButtons[i];
			myButtons[i] = nullptr;
	}
	myButtons.clear();

	for (size_t i = 0; i < mySprites.size(); ++i)
	{
		scene.RemoveInstance(mySprites[i]);
		delete mySprites[i];
		mySprites[i] = nullptr;
	}
	mySprites.clear();

	for (size_t i = 0; i < myButtonTexts.size(); ++i)
	{
		scene.RemoveInstance(myButtonTexts[i]);
		delete myButtonTexts[i];
		myButtonTexts[i] = nullptr;
	}
	myButtonTexts.clear();

	for (size_t i = 0; i < myTexts.size(); ++i)
	{
		scene.RemoveInstance(myTexts[i]);
		delete myTexts[i];
		myTexts[i] = nullptr;
	}
	myTexts.clear();
}

void CCanvas::Init(std::string aFilePath, CScene& aScene, bool addToScene)
{
	Document document = CJsonReader::Get()->LoadDocument(aFilePath);

	if (document.HasParseError())
		return;

	if (document.HasMember("Buttons"))
	{
		auto buttonDataArray = document["Buttons"].GetArray();
		for (unsigned int i = 0; i < buttonDataArray.Size(); ++i)
		{
			myButtonTexts.emplace_back(new CTextInstance(aScene, addToScene));
			myButtons.emplace_back(new CButton());
			InitButton(buttonDataArray[i].GetObjectW(), i, aScene);
		}
	}

	if (document.HasMember("Texts"))
	{
		auto textDataArray = document["Texts"].GetArray();
		for (unsigned int i = 0; i < textDataArray.Size(); ++i)
		{
			myTexts.emplace_back(new CTextInstance(aScene, addToScene));
			InitText(textDataArray[i].GetObjectW(), i);
		}
	}

	if (document.HasMember("Animated UI Elements"))
	{
		auto animatedDataArray = document["Animated UI Elements"].GetArray();
		for (unsigned int i = 0; i < animatedDataArray.Size(); ++i)
		{
			myAnimatedUIs.emplace_back(new CAnimatedUIElement(ASSETPATH(animatedDataArray[i]["Path"].GetString()), aScene, addToScene));
			InitAnimatedElement(animatedDataArray[i].GetObjectW(), i, aScene);
		}
	}

	if (document.HasMember("Background"))
	{
		myBackground = new CSpriteInstance(aScene, addToScene);
		InitBackground(ASSETPATH(document["Background"]["Path"].GetString()));
	}

	if (document.HasMember("Sprites"))
	{
		auto spriteDataArray = document["Sprites"].GetArray();
		for (unsigned int i = 0; i < spriteDataArray.Size(); ++i)
		{
			mySprites.emplace_back(new CSpriteInstance(aScene, addToScene));
			InitSprite(spriteDataArray[i].GetObjectW(), i);
		}
	}

	if (document.HasMember("PostmasterEvents"))
	{
		InitMessageTypes(document["PostmasterEvents"]["Events"].GetArray());
	}

}

void CCanvas::ReInit(std::string aFilePath, CScene& aScene, bool addToScene)
{
	std::cout << __FUNCTION__ << std::endl;
	Document document = CJsonReader::Get()->LoadDocument(aFilePath);

	if (document.HasParseError())
		return;

	if (document.HasMember("Buttons"))
	{
		auto buttonDataArray = document["Buttons"].GetArray();

		int currentSize = (int)myButtons.size();
		int newSize = (int)buttonDataArray.Size();
		int difference = currentSize - newSize;
		/*
		current - new = difference
			* Same:
				5 - 5 = 0
			 * More:
				5 - 6 = -1
			 * Less:
				5 - 4 = 1
		*/
		if (difference > 0)// There are fewer items than before.
		{
			CScene& scene = IRONWROUGHT->GetActiveScene();
			for (int i = newSize; i < currentSize; ++i)
			{
				delete myButtons[i];
				myButtons[i] = nullptr;
				myButtons.pop_back();

				scene.RemoveInstance(myButtonTexts[i]);
				delete myButtonTexts[i];
				myButtonTexts[i] = nullptr;
				myButtonTexts.pop_back();
			}
			currentSize = newSize;
		}
		else if (difference < 0)// There are more items than before.
		{
			for (int i = currentSize; i < newSize; ++i)
			{
				myButtonTexts.emplace_back(new CTextInstance(aScene, addToScene));
				myButtons.emplace_back(new CButton());
				InitButton(buttonDataArray[i].GetObjectW(), i, aScene);
			}
		}
		for (int i = 0; i < currentSize; ++i)
		{
			InitButton(buttonDataArray[i].GetObjectW(), i, aScene);
		}
	}

	if (document.HasMember("Texts"))
	{
		auto textDataArray = document["Texts"].GetArray();

		int currentSize = (int)myTexts.size();
		int newSize = (int)textDataArray.Size();
		int difference = currentSize - newSize;

		if (difference > 0)// There are fewer items than before.
		{
			CScene& scene = IRONWROUGHT->GetActiveScene();
			for (int i = newSize; i < currentSize; ++i)
			{
				scene.RemoveInstance(myTexts[i]);
				delete myTexts[i];
				myTexts[i] = nullptr;
				myTexts.pop_back();
			}
			currentSize = newSize;
		}
		else if (difference < 0)// There are more items than before.
		{
			for (int i = currentSize; i < newSize; ++i)
			{
				myTexts.emplace_back(new CTextInstance(aScene, addToScene));
				InitText(textDataArray[i].GetObjectW(), i);
			}
		}
		for (unsigned int i = 0; i < textDataArray.Size(); ++i)
		{
			InitText(textDataArray[i].GetObjectW(), i);
		}
	}

	if (document.HasMember("Animated UI Elements"))
	{
		auto animatedDataArray = document["Animated UI Elements"].GetArray();
		int currentSize = (int)myAnimatedUIs.size();
		int newSize = (int)animatedDataArray.Size();
		int difference = currentSize - newSize;

		if (difference > 0)// There are fewer items than before.
		{
			CScene& scene = IRONWROUGHT->GetActiveScene();
			for (int i = newSize; i < currentSize; ++i)
			{
				scene.RemoveInstance(myAnimatedUIs[i]);
				delete myAnimatedUIs[i];
				myAnimatedUIs[i] = nullptr;
				myAnimatedUIs.pop_back();
			}
			currentSize = newSize;
		}
		else if (difference < 0)// There are more items than before.
		{
			for (int i = currentSize; i < newSize; ++i)
			{
				myAnimatedUIs.emplace_back(new CAnimatedUIElement(aScene, addToScene));
				InitAnimatedElement(animatedDataArray[i].GetObjectW(), i, aScene);
			}
		}
		for (int i = 0; i < currentSize; ++i)
		{
			InitAnimatedElement(animatedDataArray[i].GetObjectW(), i, aScene);
		}
	}

	if (document.HasMember("Background"))
	{
		InitBackground(ASSETPATH(document["Background"]["Path"].GetString()));
	}

	if (document.HasMember("Sprites"))
	{
		auto spriteDataArray = document["Animated UI Elements"].GetArray();
		int currentSize = (int)mySprites.size();
		int newSize = (int)spriteDataArray.Size();
		int difference = currentSize - newSize;

		if (difference > 0)// There are fewer items than before.
		{
			CScene& scene = IRONWROUGHT->GetActiveScene();
			for (int i = newSize; i < currentSize; ++i)
			{
				scene.RemoveInstance(mySprites[i]);
				delete mySprites[i];
				mySprites[i] = nullptr;
				mySprites.pop_back();
			}
			currentSize = newSize;
		}
		else if (difference < 0)// There are more items than before.
		{
			for (int i = currentSize; i < newSize; ++i)
			{
				mySprites.emplace_back(new CSpriteInstance(aScene, addToScene));
				InitAnimatedElement(spriteDataArray[i].GetObjectW(), i, aScene);
			}
		}
		for (int i = 0; i < currentSize; ++i)
		{
			InitSprite(spriteDataArray[i].GetObjectW(), i);
		}
	}

	if (document.HasMember("PostmasterEvents"))
	{
		InitMessageTypes(document["PostmasterEvents"]["Events"].GetArray());
	}
}

void CCanvas::Update()
{
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
	{
		mySprites[0]->PlayAnimation(0);
	}

	if (Input::GetInstance()->IsMouseReleased(Input::EMouseButton::Left))
	{
		mySprites[0]->PlayAnimation(0, false, true);
	}

	if (!mySprites[0]->GetShouldAnimate() && !INPUT->IsMouseDown(Input::EMouseButton::Left))
	{
		mySprites[0]->PlayAnimation(1, true);
	}
	else if (!mySprites[0]->GetShouldAnimate()){
		mySprites[0]->Rotate(CTimer::Dt()*720.0f);
	}

	for (unsigned int i = 0; i < mySprites.size(); ++i)
	{
		mySprites[i]->Update();
	}

	if (myButtons.size() <= 0)
		return;

	DirectX::SimpleMath::Vector2 mousePos = { static_cast<float>(Input::GetInstance()->MouseX()), static_cast<float>(Input::GetInstance()->MouseY()) };
	for (unsigned int i = 0; i < myButtons.size(); ++i)
	{
		myButtons[i]->CheckMouseCollision(mousePos);
	}

	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
	{
		for (unsigned int i = 0; i < myButtons.size(); ++i)
		{
			myButtons[i]->Click(true, nullptr);
		}

		mySprites[0]->PlayAnimation(0);
	}

	if (Input::GetInstance()->IsMouseReleased(Input::EMouseButton::Left))
	{
		for (unsigned int i = 0; i < myButtons.size(); ++i)
		{
			myButtons[i]->Click(false, nullptr);
		}

		mySprites[0]->PlayAnimation(0, false, true);
	}
}

void CCanvas::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case EMessageType::PlayerHealthChanged:
		if (myAnimatedUIs[0])
		{
			myAnimatedUIs[0]->Level(*static_cast<float*>(aMessage.data));
		}
		break;
	default:
		break;
	}
}

void CCanvas::SubscribeToMessages()
{
	for (auto messageType : myMessageTypes)
	{
		CMainSingleton::PostMaster().Subscribe(messageType, this);
	}
}

void CCanvas::UnsubscribeToMessages()
{
	for (auto messageType : myMessageTypes)
	{
		CMainSingleton::PostMaster().Unsubscribe(messageType, this);
	}
}

bool CCanvas::GetEnabled()
{
	return myIsEnabled;
}

void CCanvas::SetEnabled(bool isEnabled)
{
	if (myIsEnabled != isEnabled)
	{
		myIsEnabled = isEnabled;

		for (auto button : myButtons)
		{
			button->myEnabled = myIsEnabled;
		}

		for (auto sprite : mySprites)
		{
			sprite->SetShouldRender(myIsEnabled);
		}
	}
}

bool CCanvas::InitButton(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex, CScene& aScene)
{
	myButtonTexts[anIndex]->Init(CTextFactory::GetInstance()->GetText(ASSETPATH(aRapidObject["FontAndFontSize"].GetString())));
	myButtonTexts[anIndex]->SetText(aRapidObject["Text"].GetString());
	myButtonTexts[anIndex]->SetColor({ aRapidObject["Text Color R"].GetFloat(), aRapidObject["Text Color G"].GetFloat(), aRapidObject["Text Color B"].GetFloat(), 1.0f });
	Vector2 pos = { aRapidObject["Text Position X"].GetFloat(), aRapidObject["Text Position Y"].GetFloat() };
	myButtonTexts[anIndex]->SetPosition(pos);
	myButtonTexts[anIndex]->SetPivot({ aRapidObject["Text Pivot X"].GetFloat(), aRapidObject["Text Pivot Y"].GetFloat() });

	SButtonData data;
	if (aRapidObject.HasMember("Sprite Position X"))
		data.myPosition.x = aRapidObject["Sprite Position X"].GetFloat();
	else
		data.myPosition.x = pos.x;

	if (aRapidObject.HasMember("Sprite Position Y"))
		data.myPosition.y = aRapidObject["Sprite Position Y"].GetFloat();
	else
		data.myPosition.y = pos.y;

	data.myDimensions = { aRapidObject["Pixel Width"].GetFloat(), aRapidObject["Pixel Height"].GetFloat() };
	data.mySpritePaths.at(0) = ASSETPATH(aRapidObject["Idle Sprite Path"].GetString());
	data.mySpritePaths.at(1) = ASSETPATH(aRapidObject["Hover Sprite Path"].GetString());
	data.mySpritePaths.at(2) = ASSETPATH(aRapidObject["Click Sprite Path"].GetString());

	auto messageDataArray = aRapidObject["Messages"].GetArray();
	data.myMessagesToSend.resize(messageDataArray.Size());

	for (unsigned int j = 0; j < messageDataArray.Size(); ++j)
	{
		data.myMessagesToSend[j] = static_cast<EMessageType>(messageDataArray[j].GetInt());
	}

	myButtons[anIndex]->Init(data, aScene);

	return true;
}

bool CCanvas::InitText(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex)
{
	myTexts[anIndex]->Init(CTextFactory::GetInstance()->GetText(ASSETPATH(aRapidObject["FontAndFontSize"].GetString())));
	myTexts[anIndex]->SetText(aRapidObject["Text"].GetString());
	myTexts[anIndex]->SetColor({ aRapidObject["Color R"].GetFloat(), aRapidObject["Color G"].GetFloat(), aRapidObject["Color B"].GetFloat(), 1.0f });
	myTexts[anIndex]->SetPosition({ aRapidObject["Position X"].GetFloat(), aRapidObject["Position Y"].GetFloat() });
	myTexts[anIndex]->SetPivot({ aRapidObject["Pivot X"].GetFloat(), aRapidObject["Pivot Y"].GetFloat() });

	return true;
}

bool CCanvas::InitAnimatedElement(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex, CScene & /*aScene*/)
{
	myAnimatedUIs[anIndex]->Init(ASSETPATH(aRapidObject["Path"].GetString()), true);

	float x = aRapidObject["Position X"].GetFloat();
	float y = aRapidObject["Position Y"].GetFloat();

	float sx = aRapidObject["Scale X"].GetFloat();
	float sy = aRapidObject["Scale Y"].GetFloat();
	myAnimatedUIs[anIndex]->SetPosition({ x, y });
	myAnimatedUIs[anIndex]->SetScale({ sx, sy });

	if (aRapidObject.HasMember("Level")) {
		myAnimatedUIs[anIndex]->Level(aRapidObject["Level"].GetFloat());
	}

	return true;
}

bool CCanvas::InitBackground(const std::string& aPath)
{
	myBackground->Init(CSpriteFactory::GetInstance()->GetSprite(aPath));
	myBackground->SetRenderOrder(ERenderOrder::BackgroundLayer);
	return true;
}

bool CCanvas::InitSprite(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex)
{
	Vector2 scale(1.0f, 1.0f);
	if (aRapidObject.HasMember("Scale X"))
		scale.x = aRapidObject["Scale X"].GetFloat();
	if (aRapidObject.HasMember("Scale Y"))
		scale.y = aRapidObject["Scale Y"].GetFloat();

	std::vector<SSpriteSheetPositionData> spriteAnimations;
	if (aRapidObject.HasMember("Animations"))
	{
		auto animations = aRapidObject["Animations"].GetArray();
		for (unsigned int i = 0; i < animations.Size(); ++i)
		{
			SSpriteSheetPositionData data;
			data.myAnimationName = animations[i]["Name"].GetString();
			data.mySpriteWidth = animations[i]["FrameWidth"].GetFloat();
			data.mySpriteHeight = animations[i]["FrameHeight"].GetFloat();
			data.myVerticalStartingPosition = animations[i]["VerticalStartingPos"].GetFloat();
			data.myNumberOfFrames = animations[i]["NumberOfFrames"].GetInt();
			data.mySpeedInFramesPerSecond = animations[i]["FramesPerSecond"].GetFloat();
			data.myRotationSpeedInSeconds = animations[i]["RotationSpeedPerSecond"].GetFloat();
			spriteAnimations.push_back(data);
		}
	}

	if (spriteAnimations.empty())
		mySprites[anIndex]->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(aRapidObject["Path"].GetString())), scale);
	else 
	{
		mySprites[anIndex]->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(aRapidObject["Path"].GetString())), spriteAnimations, scale);
	}

	mySprites[anIndex]->SetPosition(
		{ aRapidObject["Position X"].GetFloat() 
		, aRapidObject["Position Y"].GetFloat() 
		}
	);

	return true;
}

bool CCanvas::InitMessageTypes(const rapidjson::GenericArray<false, rapidjson::Value>& aRapidArray)
{
	UnsubscribeToMessages();
	myMessageTypes.clear();

	myMessageTypes.resize(aRapidArray.Size());
	for (unsigned int j = 0; j < aRapidArray.Size(); ++j)
	{
		myMessageTypes[j] = static_cast<EMessageType>(aRapidArray[j].GetInt());
	}
	SubscribeToMessages();

	return true;
}