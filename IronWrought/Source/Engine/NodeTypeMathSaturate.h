#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathSaturate : public CNodeType
{
public:
	CNodeTypeMathSaturate();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

