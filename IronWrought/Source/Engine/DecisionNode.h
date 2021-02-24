#pragma once
#include "BaseDecisionNode.h"

class CDecisionNode : public IBaseDecisionNode
{
public:
	CDecisionNode();
	CDecisionNode(CDecisionNode* aDecisionNodeToCopy);
	~CDecisionNode();
	virtual void* TestValue() = 0;
	virtual void TestValue(void* aValue) = 0;
	virtual IBaseDecisionNode* GetBranch() = 0;
	IBaseDecisionNode* MakeDecision() override;
	void InvalidateTree() override;

	IBaseDecisionNode* TrueNode();
	IBaseDecisionNode* FalseNode();
	void TrueNode(IBaseDecisionNode* aTrueNode);
	void FalseNode(IBaseDecisionNode* aFalseNode);

private:
	IBaseDecisionNode* myTrueNode;
	IBaseDecisionNode* myFalseNode;

};