#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVariableSetFloat : public CNodeType
{
public:
	CNodeTypeVariableSetFloat();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Set Data"; }
};
