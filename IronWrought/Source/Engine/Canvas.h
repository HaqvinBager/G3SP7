#pragma once
#include "Observer.h"

class CButton;
class CSpriteInstance;
class CTextInstance;
class CAnimatedUIElement;
class CScene;

#include "JsonReader.h"

class CCanvas : public IObserver
{
public:
	CCanvas();
	~CCanvas();

public:
	void Init(std::string aFilePath, CScene& aScene, bool addToScene = true);
	void ReInit(std::string aFilePath, CScene& aScene, bool addToScene = true);
	void Update();

public:
	void Receive(const SMessage& aMessage) override;
	void SubscribeToMessages();
	void UnsubscribeToMessages();

public:
	bool GetEnabled();
	void SetEnabled(bool isEnabled);

	std::vector<CButton*> GetButtons() { return myButtons; }
	std::vector<CTextInstance*> GetTexts() { return myTexts; }
	std::vector<CSpriteInstance*> GetSprites() { return mySprites; }
	std::vector<CAnimatedUIElement*> GetAnimatedUI() { return myAnimatedUIs; }

private:
	bool InitButton(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex, CScene& aScene);
	bool InitText(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex);
	bool InitAnimatedElement(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex, CScene& aScene);
	bool InitBackground(const std::string& aPath);
	bool InitSprite(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex);
	bool InitMessageTypes(const rapidjson::GenericArray<false, rapidjson::Value>& aRapidArray);

private:
	bool myIsEnabled;
	CSpriteInstance* myBackground;

	std::vector<CTextInstance*> myButtonTexts;
	std::vector<CButton*>		myButtons;
	std::vector<CAnimatedUIElement*> myAnimatedUIs;
	std::vector<CSpriteInstance*> mySprites;
	std::vector<CTextInstance*> myTexts;
	std::vector<EMessageType> myMessageTypes;
};