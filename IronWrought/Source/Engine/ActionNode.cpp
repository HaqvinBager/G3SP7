#include "ActionNode.h"
#include "DecisionTreeManager.h"
CActionNode::CActionNode()
{
	TreeIsComplete(true);
	ID(CDecisionTreeManager::Get()->RequestID());
	CDecisionTreeManager::Get()->AddNode(this);
}

CActionNode::~CActionNode()
{
	InvalidateTree();
}

IBaseDecisionNode* CActionNode::MakeDecision()
{
	TakeAction();
	return this;
}

void CActionNode::InvalidateTree()
{
	if (Parent() != nullptr)
	{
		Parent()->InvalidateTree();
	}
}