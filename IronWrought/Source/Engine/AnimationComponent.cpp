#include "stdafx.h"
#include "AnimationComponent.h"

#include "GameObject.h"
#include "ModelComponent.h"
#include "Model.h"
#include "AnimationController.h"
#include "Timer.h"

CAnimationComponent::CAnimationComponent(CGameObject& aParent, const std::string& aModelFilePath, std::vector<std::string>& someAnimationPaths)
	: CBehaviour(aParent)
{

#ifndef USING_TGA_ORIGINAL
	myController = new CAnimationController();
	myController->ImportRig(aModelFilePath);
	for (std::string s : someAnimationPaths)
	{
		myController->ImportAnimation(s);
	}
#else
	myController = new CAnimationController(aModelFilePath.c_str());
	myController->Import3DFromFile(aModelFilePath);
	for (std::string s : someAnimationPaths)
	{
		myController->Add3DAnimFromFile(s);
	}
#endif

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
#include <iostream>
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

const float CAnimationComponent::GetCurrentAnimationPercent()
{
#ifndef USING_TGA_ORIGINAL
	return myController->CurrentAnimationTimePercent();
#else
	return 1.1f;
#endif
}
const float CAnimationComponent::GetCurrentAnimationDuration()
{
#ifndef USING_TGA_ORIGINAL
	return myController->CurrentAnimationDuration();
#else
	return 1.1f;
#endif
}

const float CAnimationComponent::GetCurrentAnimationTicksPerSecond()
{
#ifndef USING_TGA_ORIGINAL
	return myController->CurrentAnimationTicksPerSecond();
#else
	return 1.1f;
#endif
}

void CAnimationComponent::GetAnimatedBlendTransforms(SlimMatrix44 * transforms)
{
	BoneTransformsWithBlend(transforms, myBlend.myBlendLerp);
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
void CAnimationComponent::UpdateBlended()
{
	BlendStep();
	SetBonesToIdentity();
	GetAnimatedBlendTransforms(myBones.data());
}

void CAnimationComponent::BoneTransformsWithBlend(SlimMatrix44* Transforms, float aBlendFactor)
{
	std::vector<aiMatrix4x4> trans;
#ifndef USING_TGA_ORIGINAL
	myController->SetBoneTransforms(trans);
	myController->SetBlendTime(aBlendFactor);
#else
	myController->BoneTransform(trans);
	myController->SetBlendTime(aBlendFactor);
#endif

	memcpy(&Transforms[0], &trans[0], (sizeof(float) * 16) * trans.size());
}

void CAnimationComponent::BlendStep()
{
#ifndef USING_TGA_ORIGINAL
	myController->UpdateAnimationTimes();
#else
	myController->Update();
#endif
}