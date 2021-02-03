#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathToRadians : public CNodeType
{
public:
	CNodeTypeMathToRadians();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "To Radians"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

