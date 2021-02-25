#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathToDegrees : public CNodeType
{
public:
	CNodeTypeMathToDegrees();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

