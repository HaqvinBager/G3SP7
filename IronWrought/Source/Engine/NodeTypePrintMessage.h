#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypePrintMessage : public CNodeType
{
public:
	CNodeTypePrintMessage();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Print Message"; }
	std::string GetNodeTypeCategory() override { return "Debugging"; }
};

