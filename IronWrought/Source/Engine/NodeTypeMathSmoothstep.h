#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathSmoothstep : public CNodeType
{
public:
	CNodeTypeMathSmoothstep();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Smoothstep"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

