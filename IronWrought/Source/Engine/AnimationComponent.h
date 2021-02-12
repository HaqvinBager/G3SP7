#pragma once
#include "Behaviour.h"
#include "StringID.hpp"
#include "SimpleMath.h"
#include "ModelMath.h"

class CAnimationController;

#ifdef _DEBUG
//#define ANIMATION_DEBUG
#endif
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
	CAnimationComponent(CGameObject& aParent, const std::string& aModelFilePath, std::vector<std::string>& someAnimationPaths);
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
	void BlendToAnimation(unsigned int anAnimationIndex, float aBlendDuration = 0.3f, bool anUpdateBoth = true, bool aTemporary = false, float aTime = 0.0f);
	void ToggleUseLerp(bool shouldUseLerp) { myShouldUseLerp = shouldUseLerp; }
	void BlendLerp(float aLerpValue);

public:
	const float GetBlendLerp() const { return myAnimationBlend.myBlendLerp; }

private:
	void SetBonesToIdentity();
	void UpdateBlended();

private:
	bool myShouldUseLerp;
	CAnimationController* myController;
	SAnimationBlend myAnimationBlend;
	std::array<SlimMatrix44, 64> myBones { };

// Used in SP6, optional to keep. Saves Id in vector using CStringID (int + _Debug::string).
	//std::vector<CStringID> myAnimationIds;

//////////////////////////////////////////
// Helper functions for myAnimationIds //
////////////////////////////////////////
/*
private:
	// This works if the ids are in a sorted list. But if the animations are added at random to the project the ids aren't sorted, making this function unsafe.
	//bool WithinIDRange(const int anID)
	//{
	//	return (anID <= myAnimationIds.back().ID() && anID >= myAnimationIds.front().ID()); 
	//}

	// Goes through std::vector myAnimationID: returns true if it finds the id within the list.
	//bool HasID(const int anID)
	//{
	//	for (auto& id : myAnimationIds)
	//	{
	//		if (id.ID() == anID)
	//		{
	//			return true;
	//		}
	//	}
	//	return false;
	//}

	// This works if the ids are in a sorted list. Uses first abs(id - anID) +1. Unsafe.
	//const int GetIndexFromID(const int anID)
	//{
	//	int index = abs(myAnimationIds[0].ID() - anID ) + 1;
	//	return index;
	//}

	// Checks for ID withing std::vector myAnimationIds and returns index of ID
	//const int GetIndexFromList(const int anID)
	//{
	//	for (int i = 0; i < myAnimationIds.size(); ++i)
	//	{
	//		if (myAnimationIds[i].ID() == anID)
	//		{
	//			return i + 1; // Animations inside CAnimation start at 1, 0 is a static civillian/ tpose.
	//		}
	//	}
	//	return 0;
	//}
////////////////////////////////////////////
// ! Helper functions for myAnimationIds //
//////////////////////////////////////////
*/
};

#pragma region COMMENTED 2020_11_11 UNUSED, No defintions existed (From TGA Animation project?)
//class SceneAnimator;
//public :
	//void BoneTransform(SlimMatrix44* Transforms); 
	//void SetAnimator(SceneAnimator* anAnimator) { myAnimator = anAnimator; }	
	//void SetBindPose(SceneAnimator* aBindPose) { myBindPose = aBindPose; }	
	//void SetActiveAnimations(std::vector<int>& someActiveAnimations) { myActiveAnimations = someActiveAnimations; }	
	//void SetTotalAnimationTime(float aTotalAnimationTime) { myTotalAnimationTime = aTotalAnimationTime; }		
	//void SetAnimationTime(float anAnimationTime) { myAnimTime = anAnimationTime; }		
	//void SetAnimationSpeed(int anAnimationSpeed) { myAnimSpeed = anAnimationSpeed; }		

	//SceneAnimator* GetAnimator() const { return myAnimator; }		
	//SceneAnimator* GetBindPose() const { return myBindPose; }		
	//const std::vector<int>& GetActiveAnimations() const { return myActiveAnimations; }
	//const float GetTotalAnimationTime() const { return myTotalAnimationTime; }		
	//const float GetAnimationTime() const { return myAnimTime; }		
	//const int GetAnimationSpeed() const { return myAnimSpeed; }		


//private:
	//SceneAnimator* myAnimator; 
	//SceneAnimator* myBindPose = nullptr;
	//std::vector<int> myActiveAnimations;

	//int myAnimSpeed = 60;
	//float myAnimTime = 0;

#pragma endregion ! 2020_11_11 UNUSED