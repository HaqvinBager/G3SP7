#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVariableGetInt : public CNodeType
{
public:
	CNodeTypeVariableGetInt();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Get Data"; }
};
