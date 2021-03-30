#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CPatrolActionNode;

class CNodeTypeActionPatrol : public CNodeType
{
public:
	CNodeTypeActionPatrol();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	//void ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Decision Tree"; }

private:
	std::unordered_map<CNodeInstance*, CPatrolActionNode*> myActions;
};

