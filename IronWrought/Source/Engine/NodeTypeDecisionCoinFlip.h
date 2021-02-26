#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CFlipACoinDecision;

class CNodeTypeDecisionCoinFlip : public CNodeType
{
public:
	CNodeTypeDecisionCoinFlip();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	void ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Flip A Coin Decision"; }
	std::string GetNodeTypeCategory() override { return "Decision Tree"; }

private:
	std::unordered_map<CNodeInstance*, CFlipACoinDecision*> myDecisions;
};

