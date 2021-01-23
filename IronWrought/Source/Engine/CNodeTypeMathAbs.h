#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathAbs : public CNodeType
{
public:
	CNodeTypeMathAbs();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Abs"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

