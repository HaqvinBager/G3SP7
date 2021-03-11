#pragma once
#include "BaseDecisionNode.h"

class CActionNode : public IBaseDecisionNode
{
public:
	CActionNode();
	~CActionNode();
	//Return self because is leaf
	IBaseDecisionNode* MakeDecision() override;
	void InvalidateTree() override;
	virtual void TakeAction() = 0;
private:

};