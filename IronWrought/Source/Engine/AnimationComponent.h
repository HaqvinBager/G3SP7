#pragma once
#include "Behaviour.h"
#include "StringID.hpp"
#include "SimpleMath.h"
#include "ModelMath.h"

//class SceneAnimator;
class CAnimationController;

struct SAnimationBlend
{
	int myFirst = 1;
	int mySecond = 1;
	float myBlendLerp = 0.0f;
};

class CGameObject;
class CAnimationComponent : public CBehaviour
{
public:
	CAnimationComponent(CGameObject& aParent, const std::string& aModelFilePath, std::vector<std::string>& someAnimationPaths);
	~CAnimationComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

public:
	const float GetCurrentAnimationPercent();
	const float GetCurrentAnimationDuration();
	const float GetCurrentAnimationTicksPerSecond();

	std::array<SlimMatrix44, 64> GetBones() { return myBones; }
	void GetAnimatedBlendTransforms(SlimMatrix44* transforms);

// TEMP
	CAnimationController* GetController() { return myController; }
// !TEMP
public:
	const float GetBlendLerp() const { return myBlend.myBlendLerp; }

private:// CAnimation functions
	void BoneTransformsWithBlend(SlimMatrix44* Transforms, float aBlendFactor);
	void BlendStep();

private:
	void SetBonesToIdentity();
	void UpdateBlended();
	void SetBlend(int anAnimationIndex, int anAnimationIndexTwo, float aBlend);

private:
	CAnimationController* myController;
	std::array<SlimMatrix44, 64> myBones { };
	SAnimationBlend myBlend;
	std::vector<CStringID> myAnimationIds;

private: // Needed for template functions
	// This works if the ids are in a sorted list. But if the animations are added at random to the project the ids aren't sorted, making this function unsafe.
	bool WithinIDRange(const int anID)
	{
		return (anID <= myAnimationIds.back().ID() && anID >= myAnimationIds.front().ID()); 
	}

	// Goes through std::vector myAnimationID: returns true if it finds the id within the list.
	bool HasID(const int anID)
	{
		for (auto& id : myAnimationIds)
		{
			if (id.ID() == anID)
			{
				return true;
			}
		}
		return false;
	}

	// This works if the ids are in a sorted list. Uses first abs(id - anID) +1. Unsafe.
	const int GetIndexFromID(const int anID)
	{
		int index = abs(myAnimationIds[0].ID() - anID ) + 1;
		return index;
	}

	// Checks for ID withing std::vector myAnimationIds and returns index of ID
	const int GetIndexFromList(const int anID)
	{
		for (int i = 0; i < myAnimationIds.size(); ++i)
		{
			if (myAnimationIds[i].ID() == anID)
			{
				return i + 1; // Animations inside CAnimation start at 1, 0 is a static civillian/ tpose.
			}
		}
		return 0;
	}
};

#pragma region COMMENTED 2020_11_11 UNUSED, No defintions existed
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