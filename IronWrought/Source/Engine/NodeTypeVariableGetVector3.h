#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVariableGetVector3 : public CNodeType
{
public:
	CNodeTypeVariableGetVector3();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Get Data"; }
};
