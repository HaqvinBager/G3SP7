#include "stdafx.h"
#include "FlipACoinDecision.h"



CFlipACoinDecision::CFlipACoinDecision() :
	CDecisionNode(),
	myTestValue(false),
	myTime(0.0f)
{
	std::srand(static_cast<unsigned int>(CTimer::Time()));
}

CFlipACoinDecision::CFlipACoinDecision(CFlipACoinDecision* aFlipACoinDecision) :
	CDecisionNode(aFlipACoinDecision),
	myTestValue(aFlipACoinDecision->myTestValue)
{
}

void* CFlipACoinDecision::TestValue()
{
	return &myTestValue;
}

void CFlipACoinDecision::TestValue(void* aValue)
{
	myTestValue = *(static_cast<bool*> (aValue));
}

IBaseDecisionNode* CFlipACoinDecision::GetBranch()
{
	if (myTime <= 0.0f) {
		myTime = myMaxTime;
		myTestValue = rand() % 2;
	}
	myTime -= CTimer::Dt();
	if (myTestValue == true) {
		return TrueNode();
	}
	else {
		return FalseNode();
	}
}

void CFlipACoinDecision::SetMaxTime(float aTime)
{
	myMaxTime = aTime;
}

float CFlipACoinDecision::GetMaxTime()
{
	return myMaxTime;
}
