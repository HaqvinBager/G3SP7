#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVariableSetVector3 : public CNodeType
{
public:
	CNodeTypeVariableSetVector3();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Set Data"; }
};
