#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVariableSetBool : public CNodeType
{
public:
	CNodeTypeVariableSetBool();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Set Data"; }
};