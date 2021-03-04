#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathGreater : public CNodeType
{
public:
	CNodeTypeMathGreater();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

