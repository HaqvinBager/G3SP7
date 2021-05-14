#include "stdafx.h"
#include "AnimationComponent.h"

#include "GameObject.h"
#include "ModelComponent.h"
#include "Model.h"
#include "AnimationController.h"
#include "Timer.h"

CAnimationComponent::CAnimationComponent(CGameObject& aParent, const std::string& aModelFilePath, std::vector<std::string>& someAnimationPaths, bool aUseSafeMode)
	: CBehaviour(aParent)
	, myShouldUseLerp(false)
{
	myController = new CAnimationController();
	myController->ImportRig(aModelFilePath);

	myUseSafeMode = aUseSafeMode;
	if (aUseSafeMode)
	{
		myIsSafeToPlay.resize(someAnimationPaths.size() + 1);
		for (size_t i = 0; i < someAnimationPaths.size(); ++i)
		{
			myIsSafeToPlay[i + 1] = myController->ImportAnimation(someAnimationPaths[i]);
		}
	}
	else
		for (std::string s : someAnimationPaths)
		{
			myController->ImportAnimation(s);
		}


// Used in SP6, optional to keep. Saves Id in vector using CStringID (int + _Debug::string).
	//myAnimationIds.reserve(someAnimationPaths.size());
	//for (auto& str : someAnimationPaths)
	//{
	//	myAnimationIds.emplace_back(CStringID(str, CStringIDLoader::EStringIDFiles::AnimFile));
	//}
	//myAnimationIds.shrink_to_fit();
}

CAnimationComponent::~CAnimationComponent()
{
	delete myController;
	myController = nullptr;
}

void CAnimationComponent::Awake()
{
	SetBonesToIdentity();
}

void CAnimationComponent::Start()
{}

void CAnimationComponent::Update()
{
#ifndef ANIMATION_DEBUG
	UpdateBlended();
#endif
}

void CAnimationComponent::OnEnable()
{}
void CAnimationComponent::OnDisable()
{}
#ifdef ANIMATION_DEBUG
void CAnimationComponent::StepAnimation(const float aStep)
{

	myController->UpdateAnimationTimeConstant(aStep);
	SetBonesToIdentity();

	//Calling SetBlendTime here causes AnimCtrl::myBlendTime to be used for lerping.
	if(myShouldUseLerp)
		myController->SetBlendTime(myAnimationBlend.myBlendLerp);

	std::vector<aiMatrix4x4> trans;
	myController->SetBoneTransforms(trans);
	memcpy(myBones.data(), &trans[0], (sizeof(float) * 16) * trans.size());//was memcpy

}
#endif

void CAnimationComponent::BlendLerpBetween(int anAnimationIndex0, int anAnimationIndex1, float aBlendLerp)
{
	int index0 = anAnimationIndex0 >= static_cast<int>(myController->GetNrOfAnimations()) ? static_cast<int>(myController->GetNrOfAnimations()) - 1 : anAnimationIndex0;
	int index1 = anAnimationIndex1 >= static_cast<int>(myController->GetNrOfAnimations()) ? static_cast<int>(myController->GetNrOfAnimations()) - 1 : anAnimationIndex1;

	myAnimationBlend.myFirst		= index0;
	myAnimationBlend.mySecond		= index1;
	myAnimationBlend.myBlendLerp	= aBlendLerp;
	myController->Animation0Index(index0);
	myController->Animation1Index(index1);
	myController->SetBlendTime(aBlendLerp);
	myShouldUseLerp = true;
}

void CAnimationComponent::BlendToAnimation(unsigned int anAnimationIndex, float aBlendDuration, bool anUpdateBoth, bool aTemporary, float aTimeMultiplier)
{
	unsigned int size = static_cast<unsigned int>(myController->GetNrOfAnimations());
	unsigned int index = anAnimationIndex >= size ? size - 1 : anAnimationIndex;

	myAnimationBlend.myFirst = index;
	myController->BlendToAnimation(index, anUpdateBoth, aBlendDuration, aTemporary, aTimeMultiplier);
	myShouldUseLerp = false;
}

void CAnimationComponent::BlendLerp(float aLerpValue)
{
	myAnimationBlend.myBlendLerp = aLerpValue > 1.0f ? 1.0f : aLerpValue < 0.0f ? 0.0f : aLerpValue;
}

bool CAnimationComponent::AllowAnimationRender()
{
	if (!myUseSafeMode)
		return true;

	return SafeModeCheck();
}

const bool CAnimationComponent::PlayingTemporaryAnimation()
{
	return myController->TemporaryAnimationPlaying();
}

void CAnimationComponent::SetBonesToIdentity()
{
	for (int i = 0; i < 64; i++)
	{
		myBones[i].SetIdentity();
	}
}
void CAnimationComponent::UpdateBlended()
{
	SetBonesToIdentity();
	if (!SafeModeCheck())
		return;

	myController->UpdateAnimationTimes();

	//Calling SetBlendTime here causes AnimCtrl::myBlendTime to be used for lerping.
	if(myShouldUseLerp)
		myController->SetBlendTime(myAnimationBlend.myBlendLerp);

	std::vector<aiMatrix4x4> trans;
	myController->SetBoneTransforms(trans);
	if (trans.empty())
		return;
	memcpy(myBones.data(), &trans[0], (sizeof(float) * 16) * trans.size());//was memcpy
}

void PrintAnimationError(const int anIndex1, const int anIndex2)
{
	SetConsoleColor(CONSOLE_RED);
	std::cout << "Can't play animation with index: " << anIndex1 << " or " << anIndex2 << std::endl;
	SetConsoleColor(CONSOLE_WHITE);
}
bool CAnimationComponent::SafeModeCheck()
{
	if (myUseSafeMode)
	{
		if (myShouldUseLerp)
		{
			if (!myIsSafeToPlay[myAnimationBlend.myFirst] || !myIsSafeToPlay[myAnimationBlend.mySecond])
			{
				PrintAnimationError(myAnimationBlend.myFirst, myAnimationBlend.mySecond);
				return false;
			}
		}
		else
		{
			if (!myIsSafeToPlay[myAnimationBlend.myFirst])
			{
				PrintAnimationError(myAnimationBlend.myFirst, myAnimationBlend.myFirst);
				return false;
			}
		}
	}
	return true;
}
