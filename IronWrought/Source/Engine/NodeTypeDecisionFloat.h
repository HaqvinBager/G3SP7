#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CFloatDecisionNode;

class CNodeTypeDecisionFloat : public CNodeType
{
public:
	CNodeTypeDecisionFloat();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	void ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Decision Tree"; }

private:
	std::unordered_map<CNodeInstance*, CFloatDecisionNode*> myDecisions;
};

