#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathMul : public CNodeType
{
public:
	CNodeTypeMathMul();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

