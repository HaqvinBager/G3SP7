#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathDot : public CNodeType
{
public:
	CNodeTypeMathDot();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

