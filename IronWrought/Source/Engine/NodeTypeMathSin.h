#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathSin : public CNodeType
{
public:
	CNodeTypeMathSin();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

