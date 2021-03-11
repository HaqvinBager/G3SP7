#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CFloatDecisionNode;

class CNodeTypeFloatDecision : public CNodeType
{
public:
	CNodeTypeFloatDecision();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	void ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Float Decision"; }
	std::string GetNodeTypeCategory() override { return "Decision Tree"; }

private:
	std::unordered_map<CNodeInstance*, CFloatDecisionNode*> myDecisions;
};

