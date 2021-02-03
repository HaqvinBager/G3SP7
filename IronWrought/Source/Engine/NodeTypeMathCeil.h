#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathCeil : public CNodeType
{
public:
	CNodeTypeMathCeil();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Ceil"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

