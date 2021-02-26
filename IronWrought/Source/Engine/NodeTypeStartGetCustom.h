#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeStartGetCustom : public CNodeType
{
public:
	CNodeTypeStartGetCustom();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	bool IsStartNode() override { return true; }
	std::string GetNodeTypeCategory() override { return "Start"; }
};