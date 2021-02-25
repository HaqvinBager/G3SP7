#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathAtan2 : public CNodeType
{
public:
	CNodeTypeMathAtan2();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

