#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeUtilityInvertBool : public CNodeType
{
public:
	CNodeTypeUtilityInvertBool();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Utility"; }
};

