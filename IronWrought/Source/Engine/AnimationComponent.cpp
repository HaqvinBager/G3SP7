#include "stdafx.h"
#include "AnimationComponent.h"

#include "GameObject.h"
#include "ModelComponent.h"
#include "Model.h"
#include "AnimationController.h"
#include "Timer.h"

CAnimationComponent::CAnimationComponent(CGameObject& aParent, const std::string& aModelFilePath, std::vector<std::string>& someAnimationPaths)
	: CBehaviour(aParent)
	, myShouldUseLerp(false)
{
	myController = new CAnimationController();
	myController->ImportRig(aModelFilePath);
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
	UpdateBlended();
}

void CAnimationComponent::OnEnable()
{}
void CAnimationComponent::OnDisable()
{}

void CAnimationComponent::BlendLerpBetween(int anAnimationIndex0, int anAnimationIndex1, float aBlendLerp)
{
	myAnimationBlend.myFirst		= anAnimationIndex0;
	myAnimationBlend.mySecond		= anAnimationIndex1;
	myAnimationBlend.myBlendLerp	= aBlendLerp;
	myController->Animation0Index(anAnimationIndex0);
	myController->Animation1Index(anAnimationIndex1);
	myController->SetBlendTime(aBlendLerp);
	myShouldUseLerp = true;
}

void CAnimationComponent::BlendToAnimation(unsigned int anAnimationIndex, float aBlendDuration, bool anUpdateBoth, bool aTemporary, float aTime)
{
	myController->BlendToAnimation(anAnimationIndex, anUpdateBoth, aBlendDuration, aTemporary, aTime);
	myShouldUseLerp = false;
}

void CAnimationComponent::BlendLerp(float aLerpValue)
{
	myAnimationBlend.myBlendLerp = aLerpValue > 1.0f ? 1.0f : aLerpValue < 0.0f ? 0.0f : aLerpValue;
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
	myController->UpdateAnimationTimes();
	SetBonesToIdentity();

	//Calling SetBlendTime here causes AnimCtrl::myBlendTime to be used for lerping.
	if(myShouldUseLerp)
		myController->SetBlendTime(myAnimationBlend.myBlendLerp);

	std::vector<aiMatrix4x4> trans;
	myController->SetBoneTransforms(trans);
	memmove(myBones.data(), &trans[0], (sizeof(float) * 16) * trans.size());//was memcpy
}