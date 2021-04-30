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
#include "Engine.h"
#include "Scene.h"

//#include "rapidjson\document.h"
//#include "rapidjson\istreamwrapper.h"

using namespace rapidjson;

CCanvas::CCanvas() :
	myBackground(nullptr)
	, myIsEnabled(true)
	, myIsHUDCanvas(false)
	, myCurrentRenderLayer(0)
	, myLevelToLoad("Level_1-1")
	, myCurrentWidgetIndex(-1)
{
}

CCanvas::~CCanvas()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::UpdateCrosshair, this);
	UnsubscribeToMessages();
	myMessageTypes.clear();

	for (size_t i = 0; i < myButtons.size(); ++i)
	{
		delete myButtons[i];
		myButtons[i] = nullptr;
	}
	myButtons.clear();

	for (size_t i = 0; i < myWidgets.size(); ++i)
	{
		delete myWidgets[i];
		myWidgets[i] = nullptr;
	}
}

inline const Vector2& CCanvas::Position() const
{
	return myPosition;
}

inline void CCanvas::Position(const Vector2& aPosition)
{
	myPosition = aPosition;
	UpdatePositions();
}

inline const Vector2& CCanvas::Pivot() const
{
	return myPivot;
}

inline void CCanvas::Pivot(const Vector2& aPivot)
{
	myPivot = aPivot;
	myPosition -= myPivot;
	UpdatePositions();
}

void CCanvas::ClearFromScene(CScene& aScene)
{
	aScene.RemoveInstance(myBackground);
	delete myBackground;
	myBackground = nullptr;

	for (size_t i = 0; i < myAnimatedUIs.size(); ++i)
	{
		aScene.RemoveInstance(myAnimatedUIs[i]);
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
		aScene.RemoveInstance(mySprites[i]);
		delete mySprites[i];
		mySprites[i] = nullptr;
	}
	mySprites.clear();

	for (size_t i = 0; i < myButtonTexts.size(); ++i)
	{
		aScene.RemoveInstance(myButtonTexts[i]);
		delete myButtonTexts[i];
		myButtonTexts[i] = nullptr;
	}
	myButtonTexts.clear();

	for (size_t i = 0; i < myTexts.size(); ++i)
	{
		aScene.RemoveInstance(myTexts[i]);
		delete myTexts[i];
		myTexts[i] = nullptr;
	}
	myTexts.clear();
}

void CCanvas::Init(const std::string& aFilePath, CScene& aScene, bool addToScene, const Vector2& aParentPivot, const Vector2& aParentPosition, const unsigned int& aSpriteRenderLayerOffset)
{
	Document document = CJsonReader::Get()->LoadDocument(aFilePath);

	if (document.HasParseError())
		return;

	InitPivotAndPos(document.GetObjectW(), aParentPivot, aParentPosition);

	myCurrentRenderLayer = aSpriteRenderLayerOffset;

	if (document.HasMember("Buttons"))
	{
		auto buttonDataArray = document["Buttons"].GetArray();

		int currentSize = (int)myButtons.size();
		int newSize = (int)buttonDataArray.Size();
		int difference = currentSize - newSize;

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
				myButtons.back()->SetRenderLayer(static_cast<ERenderOrder>(2 + myCurrentRenderLayer));
			}
		}
		for (int i = 0; i < currentSize; ++i)
		{
			InitButton(buttonDataArray[i].GetObjectW(), i, aScene);
			myButtons[i]->SetRenderLayer(static_cast<ERenderOrder>(2 + myCurrentRenderLayer));
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
				myAnimatedUIs.back()->SetRenderLayer(static_cast<ERenderOrder>(2 + myCurrentRenderLayer));
			}
		}
		for (int i = 0; i < currentSize; ++i)
		{
			InitAnimatedElement(animatedDataArray[i].GetObjectW(), i, aScene);
			myAnimatedUIs.back()->SetRenderLayer(static_cast<ERenderOrder>(2 + myCurrentRenderLayer));
		}
	}

	if (document.HasMember("Background"))
	{
		InitBackground(ASSETPATH(document["Background"]["Path"].GetString()), aScene);
		myBackground->SetRenderOrder(static_cast<ERenderOrder>(0 + myCurrentRenderLayer));
	}

	if (document.HasMember("Sprites"))
	{
		auto spriteDataArray = document["Sprites"].GetArray();
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
				InitSprite(spriteDataArray[i].GetObjectW(), i);
				mySprites.back()->SetRenderOrder(static_cast<ERenderOrder>(2 + myCurrentRenderLayer));
			}
		}
		for (int i = 0; i < currentSize; ++i)
		{
			InitSprite(spriteDataArray[i].GetObjectW(), i);
			mySprites.back()->SetRenderOrder(static_cast<ERenderOrder>(2 + myCurrentRenderLayer));
		}
	}

	if (document.HasMember("PostmasterEvents"))
	{
		InitMessageTypes(document["PostmasterEvents"]["Events"].GetArray());
	}

	if (document.HasMember("Widgets"))
	{
		auto widgetsArray = document["Widgets"].GetArray();
		int currentSize = (int)myWidgets.size();
		int newSize = (int)widgetsArray.Size();
		int difference = currentSize - newSize;

		if (difference > 0)// There are fewer items than before.
		{
			CScene& scene = IRONWROUGHT->GetActiveScene();
			for (int i = newSize; i < currentSize; ++i)
			{
				myWidgets[i]->ClearFromScene(scene);
				delete myWidgets[i];
				myWidgets[i] = nullptr;
				myWidgets.pop_back();
			}
			currentSize = newSize;
		}
		else if (difference < 0)// There are more items than before.
		{
			for (int i = currentSize; i < newSize; ++i)
			{
				myWidgets.push_back(new CCanvas());
				myWidgets[i]->Init(ASSETPATH(widgetsArray[i]["Path"].GetString()), aScene, true, myPivot, myPosition, 3);
				myWidgets[i]->SetEnabled(false);
			}
		}
		for (int i = 0; i < currentSize; ++i)
		{
			myWidgets[i]->Init(ASSETPATH(widgetsArray[i]["Path"].GetString()), aScene, true, myPivot, myPosition, 3);
			myWidgets[i]->SetEnabled(false);
		}
	}

	// To start Idle Crosshair Animation
	if (mySprites.empty())
		return;
	if(mySprites[0]->HasAnimations())
		mySprites[0]->PlayAnimationUsingInternalData(1);
}

void CCanvas::Update()
{
	if (!myIsEnabled)
		return;

	for (unsigned int i = 0; i < mySprites.size(); ++i)
	{
		mySprites[i]->Update();
	}

	if (myButtons.size() <= 0)
		return;

	if (myWidgets.size() > 0)// This is a quick solution. Nothing to keep.
	{
		for (unsigned short i = 0; i < myWidgets.size(); ++i)
		{
			switch (i)
			{
				case 0:
						myLevelToLoad = "Level_1-1";
				break;

				case 1:
					if(myWidgets[i]->GetEnabled())
						myLevelToLoad = "Level_1-2";
				break;

				case 2:
					if(myWidgets[i]->GetEnabled())
						myLevelToLoad = "Level_2-1";
				break;

				case 3:
					if(myWidgets[i]->GetEnabled())
						myLevelToLoad = "Level_2-2";
				break;

				case 7:
				{
					if (!myWidgets[i]->GetEnabled())
						continue;

					for (auto& button : myButtons)
					{
						button->Enabled(false);
					}
				}	
				break;
				default:
				break;
						
			}
		}
	}

	DirectX::SimpleMath::Vector2 mousePos = { static_cast<float>(Input::GetInstance()->MouseX()), static_cast<float>(Input::GetInstance()->MouseY()) };
	for (unsigned int i = 0; i < myButtons.size(); ++i)
	{
		myButtons[i]->CheckMouseCollision(mousePos);
	}

	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
	{
		for (unsigned int i = 0; i < myButtons.size(); ++i)
		{
			myButtons[i]->Click(true, &myLevelToLoad);
		}
	}

	if (Input::GetInstance()->IsMouseReleased(Input::EMouseButton::Left))
	{
		for (unsigned int i = 0; i < myButtons.size(); ++i)
		{
			myButtons[i]->Click(false, &myLevelToLoad);
		}
	}

	for (auto& widget : myWidgets)
	{
		widget->Update();
	}
}

void CCanvas::Receive(const SMessage& aMessage)
{
	if (myIsHUDCanvas)
	{
		switch (aMessage.myMessageType)
		{
			case EMessageType::PlayerHealthChanged:
			{
				if (myAnimatedUIs.size() > 0)
				{
					if (myAnimatedUIs[0])
						myAnimatedUIs[0]->Level(*static_cast<float*>(aMessage.data));
				}
			}
			break;

			case EMessageType::UpdateCrosshair:
			{
				if (mySprites.empty())
					return;
				PostMaster::SCrossHairData* aData = reinterpret_cast<PostMaster::SCrossHairData*>(aMessage.data);
				mySprites[0]->PlayAnimationUsingInternalData(aData->myIndex, aData->myShouldBeReversed);
			}break;

			default:
				break;
		}
	}
	else
	{
		switch (aMessage.myMessageType)
		{
			case EMessageType::ToggleWidget:
			{
				if (myWidgets.empty())
					return;

				int index = *static_cast<int*>(aMessage.data);
				if (index > -1 && index < myWidgets.size())
				{
					DisableWidgets(index == myCurrentWidgetIndex ? index : 999);

					myCurrentWidgetIndex = index;
					CCanvas& widget = *myWidgets[myCurrentWidgetIndex];
					widget.SetEnabled(!widget.GetEnabled());
				}
			}break;

			default:
			break;
		}
	}

	// Not sure how we are supposed to handle this:
	for (auto& messageType : myMessageTypes)
	{
		if (aMessage.myMessageType == messageType)
		{
			// ???
			break;
		}
	}
}

void CCanvas::SubscribeToMessages()
{
	for (auto messageType : myMessageTypes)
	{
		CMainSingleton::PostMaster().Subscribe(messageType, this);
	}

	CMainSingleton::PostMaster().Subscribe(EMessageType::ToggleWidget, this);
}

void CCanvas::UnsubscribeToMessages()
{
	for (auto messageType : myMessageTypes)
	{
		CMainSingleton::PostMaster().Unsubscribe(messageType, this);
	}

	CMainSingleton::PostMaster().Unsubscribe(EMessageType::ToggleWidget, this);
}

bool CCanvas::GetEnabled()
{
	return myIsEnabled;
}

void CCanvas::SetEnabled(bool isEnabled)
{
	myIsEnabled = isEnabled;

	for (auto& button : myButtons)
		button->Enabled(myIsEnabled);

	for (auto& text : myButtonTexts)
		text->SetShouldRender(myIsEnabled);

	for (auto& sprite : mySprites)
		sprite->SetShouldRender(myIsEnabled);

	for (auto& text : myTexts)
		text->SetShouldRender(myIsEnabled);

	for (auto& animUI : myAnimatedUIs)
		animUI->SetShouldRender(myIsEnabled);

	if(myBackground)
		myBackground->SetShouldRender(myIsEnabled);
}

void CCanvas::ForceEnabled(const bool& anIsEnabled)
{
	myIsEnabled = anIsEnabled;

	for (auto& button : myButtons)
		button->Enabled(myIsEnabled);

	for (auto& text : myButtonTexts)
		text->SetShouldRender(myIsEnabled);

	for (auto& sprite : mySprites)
		sprite->SetShouldRender(myIsEnabled);

	for (auto& text : myTexts)
		text->SetShouldRender(myIsEnabled);

	for (auto& animUI : myAnimatedUIs)
		animUI->SetShouldRender(myIsEnabled);

	if(myBackground)
		myBackground->SetShouldRender(myIsEnabled);
}

void CCanvas::DisableWidgets(const int& anExceptionIndex)
{
	for (int i = 0; i < myWidgets.size(); ++i)
	{
		if (i == anExceptionIndex)
			continue;

		auto& widget = *myWidgets[i];
		widget.SetEnabled(false);
		widget.DisableWidgets();
	}
}

bool CCanvas::InitPivotAndPos(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const Vector2& aParentPivot, const Vector2& aParentPosition)
{
	myPivot.x = aRapidObject.HasMember("Pivot X") ? aRapidObject["Pivot X"].GetFloat() : 0.0f;
	myPivot.y = aRapidObject.HasMember("Pivot Y") ? aRapidObject["Pivot Y"].GetFloat() : 0.0f;
	myPosition.x = aRapidObject.HasMember("Position X") ? aRapidObject["Position X"].GetFloat() : 0.0f;
	myPosition.y = aRapidObject.HasMember("Position Y") ? aRapidObject["Position Y"].GetFloat() : 0.0f;

	myPivot += aParentPivot;
	myPosition += aParentPosition + myPivot;
	return true;
}

bool CCanvas::InitButton(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex, CScene& aScene)
{
	myButtonTexts[anIndex]->Init(CTextFactory::GetInstance()->GetText(ASSETPATH(aRapidObject["FontAndFontSize"].GetString())));
	myButtonTexts[anIndex]->SetText(aRapidObject["Text"].GetString());
	myButtonTexts[anIndex]->SetColor({ aRapidObject["Text Color R"].GetFloat(), aRapidObject["Text Color G"].GetFloat(), aRapidObject["Text Color B"].GetFloat(), 1.0f });
	Vector2 pos = { aRapidObject["Text Position X"].GetFloat(), aRapidObject["Text Position Y"].GetFloat() };
	pos += myPosition;
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

	data.myPosition += myPosition;

	data.myDimensions = { aRapidObject["Pixel Width"].GetFloat(), aRapidObject["Pixel Height"].GetFloat() };
	data.mySpritePaths.at(0) = ASSETPATH(aRapidObject["Idle Sprite Path"].GetString());
	data.mySpritePaths.at(1) = ASSETPATH(aRapidObject["Hover Sprite Path"].GetString());
	data.mySpritePaths.at(2) = ASSETPATH(aRapidObject["Click Sprite Path"].GetString());
	
	if (aRapidObject.HasMember("Messages"))
	{
		auto messagesArray = aRapidObject["Messages"].GetArray();
		data.myMessagesToSend.resize(messagesArray.Size());

		for (unsigned int j = 0; j < messagesArray.Size(); ++j)
		{
			data.myMessagesToSend[j] = static_cast<EMessageType>(messagesArray[j].GetInt());
		}
	}

	data.myWidgetToToggleIndex = -1;
	if (aRapidObject.HasMember("Sub Canvas Toggle Index"))
		data.myWidgetToToggleIndex = aRapidObject["Sub Canvas Toggle Index"].GetInt();

 	myButtons[anIndex]->Init(data, aScene);

	return true;
}

bool CCanvas::InitText(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex)
{
	myTexts[anIndex]->Init(CTextFactory::GetInstance()->GetText(ASSETPATH(aRapidObject["FontAndFontSize"].GetString())));
	myTexts[anIndex]->SetText(aRapidObject["Text"].GetString());
	myTexts[anIndex]->SetColor({ aRapidObject["Color R"].GetFloat(), aRapidObject["Color G"].GetFloat(), aRapidObject["Color B"].GetFloat(), 1.0f });
	Vector2 position = { aRapidObject["Position X"].GetFloat(), aRapidObject["Position Y"].GetFloat() };
	position += myPosition;
	myTexts[anIndex]->SetPosition(position);
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
	myAnimatedUIs[anIndex]->SetPosition(Vector2( x, y ) + myPosition);
	myAnimatedUIs[anIndex]->SetScale({ sx, sy });

	if (aRapidObject.HasMember("Level")) {
		myAnimatedUIs[anIndex]->Level(aRapidObject["Level"].GetFloat());
	}

	return true;
}

bool CCanvas::InitBackground(const std::string& aPath, CScene& aScene)
{
	if (!myBackground)
		myBackground = new CSpriteInstance(aScene);

	myBackground->Init(CSpriteFactory::GetInstance()->GetSprite(aPath));
	myBackground->SetRenderOrder(ERenderOrder::Layer0);
	myBackground->SetPosition(myPosition);
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
			data.myFramesOffset = animations[i]["FrameOffset"].GetInt();
			data.mySpeedInFramesPerSecond = animations[i]["FramesPerSecond"].GetFloat();
			data.myRotationSpeedInSeconds = animations[i]["RotationSpeedPerSecond"].GetFloat();
			data.myIsLooping = animations[i].HasMember("ShouldLoop") ? animations[i]["ShouldLoop"].GetBool() : false;
			data.myTransitionToIndex = animations[i].HasMember("TransitionIndex") ? animations[i]["TransitionIndex"].GetInt() : -1;
			data.myReverseTransitionToIndex = animations[i].HasMember("ReverseTransitionIndex") ? animations[i]["ReverseTransitionIndex"].GetInt() : -1;
			spriteAnimations.push_back(data);
		}
		// Due to being used for both Init and Reinit we need to make sure we do not add to observer list twice
		CMainSingleton::PostMaster().Unsubscribe(EMessageType::UpdateCrosshair, this);
		CMainSingleton::PostMaster().Subscribe(EMessageType::UpdateCrosshair, this);
	}

	if (spriteAnimations.empty())
		mySprites[anIndex]->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(aRapidObject["Path"].GetString())), scale);
	else 
	{
		mySprites[anIndex]->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(aRapidObject["Path"].GetString())), spriteAnimations, scale);
	}

	mySprites[anIndex]->SetPosition(Vector2(aRapidObject["Position X"].GetFloat(), aRapidObject["Position Y"].GetFloat()) + myPosition);

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

bool CCanvas::InitWidgets(const rapidjson::GenericArray<false, rapidjson::Value>& aRapidArray, CScene& aScene)
{
	if (!myWidgets.empty())
		return false;

	if (aRapidArray.Size() <= 0)
		return false;

	myWidgets.resize((size_t)aRapidArray.Size(), new CCanvas());
	for (int i = 0; i < myWidgets.size(); ++i)
	{
		myWidgets[i]->Init(ASSETPATH(aRapidArray[i]["Path"].GetString()), aScene, true, myPivot, myPosition);
		myWidgets[i]->SetEnabled(false);
	}

	return true;
}

void CCanvas::UpdatePositions()
{
	for (auto& sprite : mySprites)
	{
		sprite->SetPosition(sprite->GetPosition() - myPosition);
	}

	for (auto& animated : myAnimatedUIs)
	{
		animated->SetPosition(animated->GetPosition() - myPosition);
	}

	for (auto& button : myButtons)
	{
		auto& sprite1 = button->mySprites[0];
		auto& sprite2 = button->mySprites[1];
		auto& sprite3 = button->mySprites[2];
		sprite1->SetPosition(sprite1->GetPosition() - myPosition);
		sprite2->SetPosition(sprite2->GetPosition() - myPosition);
		sprite3->SetPosition(sprite3->GetPosition() - myPosition);
	}

	for (auto& text : myButtonTexts)
	{
		text->SetPosition(text->GetPosition() - myPosition);
	}

	for (auto& text : myTexts)
	{
		text->SetPosition(text->GetPosition() - myPosition);
	}

	myBackground->SetPosition(myBackground->GetPosition() - myPosition);

	for (auto& canvas : myWidgets)
	{
		canvas->Position(canvas->Position() - myPosition);
	}
}
