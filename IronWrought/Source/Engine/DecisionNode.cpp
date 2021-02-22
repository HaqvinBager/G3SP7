#include "DecisionNode.h"
#include <iostream>
#include "DecisionTreeManager.h"

CDecisionNode::CDecisionNode() :
	myTrueNode(nullptr),
	myFalseNode(nullptr)
{
	ID(CDecisionTreeManager::Get()->RequestID());
	CDecisionTreeManager::Get()->AddNode(this);
}

CDecisionNode::CDecisionNode(CDecisionNode* aDecisionNodeToCopy) :
	myTrueNode(aDecisionNodeToCopy->myTrueNode),
	myFalseNode(aDecisionNodeToCopy->myFalseNode)
{
	ID(CDecisionTreeManager::Get()->RequestID());
	CDecisionTreeManager::Get()->AddNode(this);
}

CDecisionNode::~CDecisionNode()
{
	InvalidateTree();
	myTrueNode = nullptr;
	myFalseNode = nullptr;
}

IBaseDecisionNode* CDecisionNode::MakeDecision()
{
	if (!TreeIsComplete())
	{
		std::cout << "Trying to run an incomplete tree" << std::endl;
		return nullptr;
	}

	IBaseDecisionNode* result = GetBranch();
	return result->MakeDecision();
}

void CDecisionNode::InvalidateTree()
{
	if (Parent() != nullptr)
	{
		Parent()->TreeIsComplete(false);
		Parent()->InvalidateTree();
	}
}

IBaseDecisionNode* CDecisionNode::TrueNode()
{
	return myTrueNode;
}

IBaseDecisionNode* CDecisionNode::FalseNode()
{
	return myFalseNode;
}

void CDecisionNode::TrueNode(IBaseDecisionNode* aTrueNode)
{
	if (myFalseNode != nullptr)
		TreeIsComplete(true);
	myTrueNode = aTrueNode;
}

void CDecisionNode::FalseNode(IBaseDecisionNode* aFalseNode)
{
	if (myTrueNode != nullptr)
		TreeIsComplete(true);
	myFalseNode = aFalseNode;
}