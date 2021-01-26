#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathDot : public CNodeType
{
public:
	CNodeTypeMathDot();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Dot"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

