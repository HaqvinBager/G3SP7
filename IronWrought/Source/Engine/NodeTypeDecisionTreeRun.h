#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class IBaseDecisionNode;

class CNodeTypeDecisionTreeRun : public CNodeType
{
public:
	CNodeTypeDecisionTreeRun();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	void ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Decision Tree"; }

private:
	std::unordered_map<CNodeInstance*, IBaseDecisionNode*> myTreesToRun;
};