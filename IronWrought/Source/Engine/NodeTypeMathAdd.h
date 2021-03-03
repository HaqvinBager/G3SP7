#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathAdd : public CNodeType
{
public:
	CNodeTypeMathAdd();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

