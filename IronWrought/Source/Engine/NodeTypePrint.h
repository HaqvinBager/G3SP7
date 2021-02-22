#pragma once
#include "NodeType.h"
#include "NodeTypes.h"
class CNodeTypePrint : public CNodeType
{
public:
	CNodeTypePrint();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Print"; }
	std::string GetNodeTypeCategory() override { return "Debugging"; }
};

