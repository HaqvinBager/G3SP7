#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeDebugPrint : public CNodeType
{
public:
	CNodeTypeDebugPrint();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Debug"; }
};

