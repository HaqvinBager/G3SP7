#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathSub : public CNodeType
{
public:
	CNodeTypeMathSub();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Sub"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

