#pragma once
#include "PostMaster.h"
#include "SpriteInstance.h"

class CCanvas;
class CSpriteInstance;
class CScene;

struct SButtonData {
	std::vector<EMessageType> myMessagesToSend;
	std::array<std::string, 3> mySpritePaths;
	// POSITION IS IN SHADER SPACE -1 -> 1 RIGHT NOW
	DirectX::SimpleMath::Vector2 myPosition;
	DirectX::SimpleMath::Vector2 myDimensions;
	std::string myText;
	int myWidgetToToggleIndex;
};

struct SUIRect {
	float myTop, myBottom, myLeft, myRight;
};

enum class EButtonState {
	Idle,
	Hover,
	Click
};

class CButton {
	friend CCanvas;

public:
	void OnHover();
	void OnClickDown();
	void OnClickUp(void* someData);
	void OnLeave();

	void Click(bool anIsPressed, void* someData);
	void CheckMouseCollision(DirectX::SimpleMath::Vector2 aScreenSpacePosition);

	std::vector<EMessageType> GetMessagesToSend() { return myMessagesToSend; }

	void Enabled(const bool& anIsEnabled);
	inline const bool Enabled();

	void SetRenderLayer(const ERenderOrder& aRenderLayer);

private:
	CButton();
	CButton(SButtonData& someData, CScene& aScene);
	~CButton();
	void Init(SButtonData& someData, CScene& aScene);

private:
	std::vector<EMessageType> myMessagesToSend;
	std::vector<std::string> myMessageData;
	std::array<CSpriteInstance*, 3> mySprites = { nullptr, nullptr, nullptr };
	SUIRect myRect;
	EButtonState myState;
	bool myEnabled;
	bool myIsMouseHover;
	int myWidgetToToggleIndex;
	
};

