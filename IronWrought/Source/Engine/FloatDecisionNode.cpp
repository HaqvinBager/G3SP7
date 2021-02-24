#include <string>
#include <iostream>
#include "FloatDecisionNode.h"

CFloatDecisionNode::CFloatDecisionNode() :
	CDecisionNode(),
	myMaxValue(0.0f),
	myMinValue(0.0f),
	myTestValue(0.0f)
{
}

CFloatDecisionNode::CFloatDecisionNode(CFloatDecisionNode* aFloatDecisionNodeToCopy) :
	CDecisionNode(aFloatDecisionNodeToCopy),
	myMaxValue(aFloatDecisionNodeToCopy->myMaxValue),
	myMinValue(aFloatDecisionNodeToCopy->myMinValue),
	myTestValue(aFloatDecisionNodeToCopy->myTestValue)
{
}

void* CFloatDecisionNode::TestValue()
{
	return &myTestValue;
}

void CFloatDecisionNode::TestValue(void* aValue)
{
	myTestValue = *(static_cast<float*>(aValue));
}

float CFloatDecisionNode::MaxValue()
{
	return myMaxValue;
}

void CFloatDecisionNode::MaxValue(float aMaxValue)
{
	myMaxValue = aMaxValue;
}

float CFloatDecisionNode::MinValue()
{
	return myMinValue;
}

void CFloatDecisionNode::MinValue(float aMinValue)
{
	myMinValue = aMinValue;
}

IBaseDecisionNode* CFloatDecisionNode::GetBranch()
{
	float temp;
	temp = myMaxValue - myTestValue;

	if (temp >= myMinValue)
	{
		return TrueNode();
	}
	else
	{
		return FalseNode();
	}
}