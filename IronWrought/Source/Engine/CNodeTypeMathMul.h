#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathMul : public CNodeType
{
public:
	CNodeTypeMathMul();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Mul"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

