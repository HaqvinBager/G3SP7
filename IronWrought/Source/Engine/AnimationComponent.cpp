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
	myController = new CAnimationController();
	myController->ImportRig(aModelFilePath);
	for (std::string s : someAnimationPaths)
	{
		myController->ImportAnimation(s);
	}
}

CAnimationComponent::~CAnimationComponent()
{
	delete myController;
	myController = nullptr;
}
void CAnimationComponent::OnEnable() { }
void CAnimationComponent::Start() { }
void CAnimationComponent::OnDisable() { }

void CAnimationComponent::Awake()
{
	SetBonesToIdentity();
}


void CAnimationComponent::Update()
{
	SetBonesToIdentity();
	myController->UpdateAnimationTimes(myBones);
}

void CAnimationComponent::SetBonesToIdentity()
{
	for (int i = 0; i < 64; i++)
	{
		myBones[i].SetIdentity();
	}
}

