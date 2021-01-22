#include "stdafx.h"
#include "AnimationComponent.h"

#include "GameObject.h"
#include "ModelComponent.h"
#include "Model.h"
#include "Animation.h"
#include "AnimationController.h"//TEMP
#include "Timer.h"

CAnimationComponent::CAnimationComponent(CGameObject& aParent, const std::string& aModelFilePath, std::vector<std::string>& someAnimationPaths)
	: CBehaviour(aParent)
	, myAnimationSpeed(1.0f)
	, myIsLooping(false)
{
	myAnimation = new CAnimation();
	myAnimation->Init(aModelFilePath.c_str(), someAnimationPaths);
	myAnimationIds.reserve(someAnimationPaths.size());
	for (auto& str : someAnimationPaths)
	{
		myAnimationIds.emplace_back(CStringID(str, CStringIDLoader::EStringIDFiles::AnimFile));
	}
	myAnimationIds.shrink_to_fit();
}

CAnimationComponent::~CAnimationComponent()
{
	delete myAnimation;
	myAnimation = nullptr;
}
#include <iostream>
void CAnimationComponent::Awake()
{
#ifdef _DEBUG
	for (auto& strID : myAnimationIds)
	{
		std::cout << __FUNCTION__ << "  " << strID.String() << " " << strID.ID() << std::endl;
	}
#endif
	SetBonesToIdentity();
	myAnimation->SetCurAnimationScene(0);
}

void CAnimationComponent::Start()
{}

void CAnimationComponent::Update()
{
	float dt = CTimer::Dt();

#ifdef USING_BLENDED_ANIMATIONS
	UpdateBlended(dt);
#else
	UpdateNonBlended(dt);
#endif
}

void CAnimationComponent::OnEnable()
{}
void CAnimationComponent::OnDisable()
{}

const float CAnimationComponent::GetCurrentAnimationPercent()
{
	return myAnimation->GetMyController().CurrentAnimationTimePercent();
}
const float CAnimationComponent::GetCurrentAnimationDuration()
{
	return myAnimation->GetMyController().CurrentAnimationDuration();
}

const float CAnimationComponent::GetCurrentAnimationTicksPerSecond()
{
	return myAnimation->GetMyController().CurrentAnimationTicksPerSecond();
}

void CAnimationComponent::GetAnimatedBlendTransforms(float dt, SlimMatrix44 * transforms)
{
	dt;
	myAnimation->BoneTransformsWithBlend(transforms, myBlend.myBlendLerp);
}
void CAnimationComponent::GetAnimatedTransforms(float dt, SlimMatrix44 * transforms)
{
	dt;
	myAnimation->BoneTransforms(transforms, myAnimationSpeed);
}

void CAnimationComponent::SetBlend(int anAnimationIndex, int anAnimationIndexTwo, float aBlend)
{
	myBlend.myFirst		= anAnimationIndex;
	myBlend.mySecond	= anAnimationIndexTwo;
	myBlend.myBlendLerp = aBlend;
}

void CAnimationComponent::SetBonesToIdentity()
{
	for (int i = 0; i < 64; i++)
	{
		myBones[i].SetIdentity();
	}
}
void CAnimationComponent::UpdateBlended(const float dt)
{
	myAnimation->BlendStep(dt);
	SetBonesToIdentity();
	GetAnimatedBlendTransforms(dt, myBones.data());
}
void CAnimationComponent::UpdateNonBlended(const float dt)
{
	myAnimation->Step();
	SetBonesToIdentity();
	GetAnimatedTransforms(dt, myBones.data());
}