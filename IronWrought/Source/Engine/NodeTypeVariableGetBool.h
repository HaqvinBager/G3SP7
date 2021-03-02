#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVariableGetBool : public CNodeType
{
public:
	CNodeTypeVariableGetBool();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Get Data"; }
};
