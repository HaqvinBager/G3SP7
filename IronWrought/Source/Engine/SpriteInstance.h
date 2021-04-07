#pragma once
#include <SimpleMath.h>// Added for ModelViewer solution, gave error here... :/

class CSprite;
class CScene;

enum class ERenderOrder {
	BackgroundLayer,
	Layer1,
	ForegroundLayer,
	PauseLayer
};

struct SSpriteSheetPositionData
{
	float mySpriteWidth;
	float mySpriteHeight;
	float myVerticalStartingPosition;
	float mySpeedInFramesPerSecond;
	int myNumberOfFrames;

	std::string myAnimationName = "";
};

struct SSpriteAnimationData
{
	unsigned int myFramesOffset;
	unsigned int myNumberOfFrames;
	float myFramesPerSecond;
	std::string myAnimationName;
};

class CSpriteInstance
{
public:
	CSpriteInstance(CScene& aScene, bool aAddToScene = true);
	CSpriteInstance();
	~CSpriteInstance();

	bool Init(CSprite* aSprite, const Vector2& aScale = {1.0f,1.0f});
	bool Init(CSprite* aSprite, const std::vector<SSpriteSheetPositionData>& someSpriteSheetPositionData, const Vector2& aScale = { 1.0f, 1.0f });
	void SetPosition(DirectX::SimpleMath::Vector2 aPosition);
	void SetNormalPosition(DirectX::SimpleMath::Vector2 aPosition);
	void SetColor(DirectX::SimpleMath::Vector4 aColor);
	void SetUVRect(DirectX::SimpleMath::Vector4 aUVRect);
	void SetSize(DirectX::SimpleMath::Vector2 aSize);
	void SetShouldRender(bool aBool);

	void Update();

	void PlayAnimation(unsigned int anIndex, bool aShouldLoop = false, bool aShouldBeReversed = false);
	void PlayAnimation(std::string aName, bool aShouldLoop = false, bool aShouldBeReversed = false);

public:
	const DirectX::SimpleMath::Vector4 GetPosition() const { return myPosition; }
	const DirectX::SimpleMath::Vector4 GetColor() const { return myColor; }
	const DirectX::SimpleMath::Vector4 GetUVRect() const { return myUVRect; }
	const DirectX::SimpleMath::Vector2 GetSize() const { return mySize; }
	bool GetShouldRender() const { return myShouldRender; }
	CSprite* GetSprite() const { return mySprite; }

	ERenderOrder GetRenderOrder() const { return myRenderOrder; }
	void SetRenderOrder(ERenderOrder aRenderOrder);

private:
	std::vector<Vector4> myAnimationFrames;
	std::vector<SSpriteAnimationData> myAnimationData;
	//std::vector<std::vector<Vector4>> myAnimations;
	DirectX::SimpleMath::Vector4 myPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::SimpleMath::Vector4 myColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::SimpleMath::Vector4 myUVRect = { 0.0f, 0.0f, 1.0f, 1.0f };
	DirectX::SimpleMath::Vector2 mySize = { 1.0f, 1.0f };
	CSprite* mySprite;
	ERenderOrder myRenderOrder;
	float myAnimationTimer;
	float myCurrentAnimationSpeed;
	unsigned int myCurrentAnimationIndex;
	unsigned int myCurrentAnimationFrame;
	bool myShouldRender = true;
	bool myShouldAnimate;
	bool myShouldLoopAnimation;
	bool myShouldReverseAnimation;
};

