#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathMax: public CNodeType
{
public:
	CNodeTypeMathMax();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

