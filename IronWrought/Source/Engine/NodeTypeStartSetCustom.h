#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeStartSetCustom : public CNodeType
{
public:
	CNodeTypeStartSetCustom();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Start"; }
};