#pragma once
#include "DecisionNode.h"

class CFlipACoinDecision : public CDecisionNode
{
public:

	CFlipACoinDecision();
	CFlipACoinDecision(CFlipACoinDecision* aCoin);

	void* TestValue() override;
	void TestValue(void* aValue) override;
	
	IBaseDecisionNode* GetBranch() override;
	void SetMaxTime(float aTime);
	float GetMaxTime();
	
private:
	bool myTestValue;
	float myTime;
	float myMaxTime;
};

