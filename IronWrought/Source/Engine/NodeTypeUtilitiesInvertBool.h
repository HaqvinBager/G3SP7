#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeUtilitiesInvertBool : public CNodeType
{
public:
	CNodeTypeUtilitiesInvertBool();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Utility"; }
};

