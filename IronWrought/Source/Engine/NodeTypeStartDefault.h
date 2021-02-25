#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeStartDefault : public CNodeType
{
public:
	CNodeTypeStartDefault();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	bool IsStartNode() override { return true; }
	std::string GetNodeTypeCategory() override { return "Start"; }
};

