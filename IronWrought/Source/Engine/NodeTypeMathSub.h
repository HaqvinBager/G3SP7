#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathSub : public CNodeType
{
public:
	CNodeTypeMathSub();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

