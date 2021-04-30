#pragma once
#include "Behaviour.h"
#include "StringID.hpp"
#include "SimpleMath.h"
#include "ModelMath.h"

class CAnimationController;

#ifdef _DEBUG
//#define ANIMATION_DEBUG
#endif

//#define ANIMATION_SAFE_MODE// DIDNT WORK FOR SOME FREAKING REASON, toggling it does not update the checks in AnimationComponent.cpp

struct SAnimationBlend
{
	int myFirst = 1;
	int mySecond = 1;
	float myBlendLerp = 10.0f;
};

class CGameObject;
class CAnimationComponent : public CBehaviour
{
public:// Component/ Behaviour inherited
#ifdef _DEBUG
	CAnimationComponent(CGameObject& aParent, const std::string& aModelFilePath, std::vector<std::string>& someAnimationPaths, bool aUseSafeMode = true);
#else
	CAnimationComponent(CGameObject& aParent, const std::string& aModelFilePath, std::vector<std::string>& someAnimationPaths, bool aUseSafeMode = false);
#endif
	~CAnimationComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

#ifdef ANIMATION_DEBUG
	void StepAnimation(const float aStep = 1.0f);
#endif

public:
	std::array<SlimMatrix44, 64> GetBones() { return myBones; }

	void BlendLerpBetween(int anAnimationIndex0, int anAnimationIndex1, float aBlendLerp);
	void BlendToAnimation(unsigned int anAnimationIndex, float aBlendDuration = 0.3f, bool anUpdateBoth = true, bool aTemporary = false, float aTimeMultiplier = 1.0f);
	void ToggleUseLerp(bool shouldUseLerp) { myShouldUseLerp = shouldUseLerp; }
	void BlendLerp(float aLerpValue);

	bool AllowAnimationRender();

	const bool PlayingTemporaryAnimation();

public:
	const float GetBlendLerp() const { return myAnimationBlend.myBlendLerp; }

private:
	void SetBonesToIdentity();
	void UpdateBlended();
	bool SafeModeCheck();

private:
	bool myShouldUseLerp;
	CAnimationController* myController;
	SAnimationBlend myAnimationBlend;
	std::array<SlimMatrix44, 64> myBones{ };

	bool myUseSafeMode;
	//#ifdef ANIMATION_SAFE_MODE // Didnt work for some reason?
	std::vector<bool> myIsSafeToPlay;
};