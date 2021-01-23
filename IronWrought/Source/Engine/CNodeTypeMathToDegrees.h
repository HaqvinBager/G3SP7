#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathToDegrees : public CNodeType
{
public:
	CNodeTypeMathToDegrees();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "To Degrees"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

