#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeUtilitiesBranching : public CNodeType
{
public:
	CNodeTypeUtilitiesBranching();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Set Data"; }
};

