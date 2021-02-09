#include "stdafx.h"
#include "Animation.h"

CAnimation::CAnimation()
	: myController(nullptr)
	, myTotalAnimationTime(0)
{}
CAnimation::~CAnimation()
{
	delete myController;
	myController = nullptr;
}

void CAnimation::Init(const char* /*aRig*/, std::vector<std::string>& /*somePathsToAnimations*/)
{
	//myController = new CAnimationController();
	//myController->ImportRig(aRig);
	//for (std::string s : somePathsToAnimations)
	//{
	//	myController->ImportAnimation(s);
	//}

	//myController->Release();
	//myController->SetAnimIndex(1, true, 0.0f);
	//myController->SetAnimIndex(2, true, 5.0f);
}
void CAnimation::BoneTransformsWithBlend(SlimMatrix44* /*Transforms*/, float /*aBlendFactor*/)
{
	//std::vector<aiMatrix4x4> trans;
	//myController->SetBoneTransforms(trans);
	//myController->SetBlendTime(aBlendFactor);
	//
	//memcpy(&Transforms[0], &trans[0], (sizeof(float) * 16) * trans.size());
}
void CAnimation::BoneTransforms(SlimMatrix44* /*Transforms*/)
{
	//std::vector<aiMatrix4x4> trans;
	//myController->SetBoneTransforms(trans);
	//if (trans.size() == 0)
	//{
	//	return;
	//}
	//memcpy(&Transforms[0], &trans[0], (sizeof(float) * 16) * trans.size());
}

void CAnimation::BlendStep(float /*aDelta*/)
{
	//myTotalAnimationTime += aDelta;
	//myController->UpdateAnimationTimes();
}

void CAnimation::Step()
{
	//THIS DOES NOTHING WTF
	/*myTotalAnimationTime += aDelta;*/
	//myController->UpdateAnimationTimes();
}

const size_t CAnimation::GetNrOfAnimations() const
{
	return (size_t)1;
	//return myController->GetNrOfAnimations(); 
}

void CAnimation::SetCurAnimationScene(const int /*aCurAnimScene*/)
{
	
	//myController->SetCurSceneIndex(aCurAnimScene);
}