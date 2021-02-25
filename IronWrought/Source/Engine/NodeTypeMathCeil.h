#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathCeil : public CNodeType
{
public:
	CNodeTypeMathCeil();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

