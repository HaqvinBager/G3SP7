#pragma once
#include "Animator.h"

class CAnimationController;

class CBlendTree
{
public:
	friend class IronWroughtImGui::CAnimator;
	
	CBlendTree(const std::string& aName, CAnimationController& aController);
	~CBlendTree();

	void Update(const float aTime);

	const std::string& Name() const { return myName; }
	const std::vector<std::string>& GetMotions() const { return myMotions; }

public:
	void AddMotion(const std::string& aMotionName) { myMotions.push_back(aMotionName); }
	void SetBlend(const float aBlendValue) { myBlendValue = aBlendValue; }

private:
//	const std::string& GetMotion(const int anIndex, const float aTime) const;

private:
	const std::string myName;
	CAnimationController& myController;

private:
	float myBlendValue;
	std::vector<std::string> myMotions;
};

