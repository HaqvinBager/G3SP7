#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeStartKeyboardInput : public CNodeType
{
public:
	CNodeTypeStartKeyboardInput();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	bool IsStartNode() override { return true; }
	std::string GetNodeTypeCategory() override { return "Start"; }
};



