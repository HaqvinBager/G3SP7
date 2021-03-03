#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathAbs : public CNodeType
{
public:
	CNodeTypeMathAbs();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

