#pragma once
#include "Behaviour.h"
#include "StringID.hpp"

#include "SimpleMath.h"
#include "ModelMath.h"

//#define USING_BLENDED_ANIMATIONS // Don't toggle this. There is no support for it atm. Anything can happen if active.

struct SAnimationBlend
{
	int myFirst = -1;
	int mySecond = -1;
	float myBlendLerp = 0;
};

class CGameObject;
class CAnimation;
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
	void GetAnimatedBlendTransforms(float dt, SlimMatrix44* transforms);
	void GetAnimatedTransforms(float dt, SlimMatrix44* transforms);

	void MovingState();
	// After calling this no other animation will be able to be called.
	bool DeadState();
	void ForceToIdleState();

	void SetIdleID(const int anIdleID);
	void SetMovingID(const int aMovingID);
	void SetDyingID(const int aDyingID);

public:
	CAnimation* GetMyAnimation() { return myAnimation; }
	const float GetBlend() const { return myBlend.myBlendLerp; }

private:
	void SetBonesToIdentity();
	void UpdateBlended(const float dt);
	void UpdateNonBlended(const float dt);
	void SetBlend(int anAnimationIndex, int anAnimationIndexTwo, float aBlend);

private:
	CAnimation* myAnimation;
	std::array<SlimMatrix44, 64> myBones { };
	SAnimationBlend myBlend;
	bool myIsLooping;
	float myAnimationSpeed;
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