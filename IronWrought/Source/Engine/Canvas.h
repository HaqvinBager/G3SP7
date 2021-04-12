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
	void ClearFromScene(CScene& aScene);

	void IsHUDCanvas(const bool& aIsHUDCanvas) { myIsHUDCanvas = aIsHUDCanvas; }
	const bool IsHUDCanvas() { return myIsHUDCanvas; }

	inline const Vector2& Position() const;
	inline void Position(const Vector2&);

	inline const Vector2& Pivot() const;
	inline void Pivot(const Vector2&);

public:
	void Init(const std::string& aFilePath, CScene& aScene, bool addToScene = true, const Vector2& aParentPivot = { 0.0f,0.0f }, const Vector2& aParentPosition = { 0.0f,0.0f }, const unsigned int& aSpriteRenderLayerOffset = 0);
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
	bool InitPivotAndPos(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const Vector2& aParentPivot = { 0.0f,0.0f }, const Vector2& aParentPosition = { 0.0f,0.0f });
	bool InitButton(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex, CScene& aScene);
	bool InitText(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex);
	bool InitAnimatedElement(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex, CScene& aScene);
	bool InitBackground(const std::string& aPath, CScene& aScene);
	bool InitSprite(const rapidjson::GenericObject<false, rapidjson::Value>& aRapidObject, const int& anIndex);
	bool InitMessageTypes(const rapidjson::GenericArray<false, rapidjson::Value>& aRapidArray);
	bool InitWidgets(const rapidjson::GenericArray<false, rapidjson::Value>& aRapidArray, CScene& aScene);

private:
	bool myIsEnabled;
	Vector2 myPosition;
	Vector2 myPivot;

	CSpriteInstance* myBackground;
	std::vector<CTextInstance*>		 myButtonTexts;// Buttons are going to have to be able to toggle Widgets owned by the Canvas, instead of message it could: "Widget Toggle":[ 1 ]
	std::vector<CButton*>			 myButtons;	  // Buttons are going to have to be able to toggle Widgets owned by the Canvas, instead of message it could: "Widget Toggle":[ 1 ]
	std::vector<CAnimatedUIElement*> myAnimatedUIs;
	std::vector<CSpriteInstance*>	 mySprites;
	std::vector<CTextInstance*>		 myTexts;
	std::vector<EMessageType>		 myMessageTypes;
	std::vector<CCanvas*>			 myWidgets;

	unsigned int myCurrentRenderLayer;

	bool myIsHUDCanvas;

	int myCurrentWidgetIndex;
};