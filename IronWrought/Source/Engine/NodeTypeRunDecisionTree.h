#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class IBaseDecisionNode;

class CNodeTypeRunDecisionTree : public CNodeType
{
public:
	CNodeTypeRunDecisionTree();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	void ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Run Tree"; }
	std::string GetNodeTypeCategory() override { return "Decision Tree"; }

private:
	std::unordered_map<CNodeInstance*, IBaseDecisionNode*> myTreesToRun;
};

