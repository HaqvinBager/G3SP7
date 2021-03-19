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
	//myController->Animation0Index(0);
	//myController->Animation1Index(0);

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
	//SetBonesToIdentity();
	//myController->UpdateAnimationTimes(myBones);
}

void CAnimationComponent::SetBonesToIdentity()
{
	for (int i = 0; i < 64; i++)
	{
		myBones[i].a1 = 1; myBones[i].a2 = 0; myBones[i].a3 = 0; myBones[i].a4 = 0;
		myBones[i].b1 = 0; myBones[i].b2 = 1; myBones[i].b3 = 0; myBones[i].b4 = 0;
		myBones[i].c1 = 0; myBones[i].c2 = 0; myBones[i].c3 = 1; myBones[i].c4 = 0;
		myBones[i].d1 = 0; myBones[i].d2 = 0; myBones[i].d3 = 0; myBones[i].d4 = 1;
	}
}

