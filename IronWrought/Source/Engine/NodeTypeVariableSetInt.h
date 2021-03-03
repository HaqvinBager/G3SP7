#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVariableSetInt : public CNodeType
{
public:
	CNodeTypeVariableSetInt();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Set Data"; }
};
