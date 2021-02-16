#pragma once
#include "DecisionNode.h"

class CFloatDecisionNode : public CDecisionNode
{
public:
	CFloatDecisionNode();
	CFloatDecisionNode(CFloatDecisionNode* aFloatDecisionNodeToCopy);
	void* TestValue() override;
	void TestValue(void* aValue) override;
	float MaxValue();
	void MaxValue(float aMaxValue);
	float MinValue();
	void MinValue(float aMinValue);
	IBaseDecisionNode* GetBranch() override;

private:
	float myMaxValue;
	float myMinValue;
	float myTestValue;
};

