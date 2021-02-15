#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CPrintActionNode;

class CNodeTypePrintAction : public CNodeType
{
public:
	CNodeTypePrintAction();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	void ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Print Action"; }
	std::string GetNodeTypeCategory() override { return "Decision Tree"; }

private:
	std::unordered_map<CNodeInstance*, CPrintActionNode*> myActions;
};

