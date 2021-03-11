#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CPrintActionNode;

class CNodeTypeActionPrint : public CNodeType
{
public:
	CNodeTypeActionPrint();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	void ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Decision Tree"; }

private:
	std::unordered_map<CNodeInstance*, CPrintActionNode*> myActions;
};

