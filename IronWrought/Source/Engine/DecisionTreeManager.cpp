#include "DecisionTreeManager.h"
#include "BaseDecisionNode.h"
#include <iostream>

CDecisionTreeManager* CDecisionTreeManager::ourInstance = nullptr;

CDecisionTreeManager::CDecisionTreeManager() : myIDCounter(0)
{
}

CDecisionTreeManager::~CDecisionTreeManager()
{
}

CDecisionTreeManager* CDecisionTreeManager::Get()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CDecisionTreeManager();
	}

	return ourInstance;
}

void CDecisionTreeManager::RunTree(IBaseDecisionNode* aRootNode)
{
	if (aRootNode != nullptr)
	{
		aRootNode->MakeDecision();
	}
}

int CDecisionTreeManager::RequestID()
{
	return ++myIDCounter;
}

void CDecisionTreeManager::AddNode(IBaseDecisionNode* aNodeToAdd)
{
	myTreeMap[aNodeToAdd->ID()] = aNodeToAdd;
}

IBaseDecisionNode* CDecisionTreeManager::GetNodeFromID(int aNodeID)
{
	return myTreeMap[aNodeID];
}

void CDecisionTreeManager::RemoveNodeWithID(int aNodeID)
{
	if (myTreeMap[aNodeID] != nullptr)
	{
		myTreeMap[aNodeID]->InvalidateTree();
	}
}