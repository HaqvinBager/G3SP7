#pragma once
#include "NodeType.h"
#include "NodeTypes.h"
#include <string>

class CNodeTypeInputGetMousePosition : public CNodeType
{
public:
	CNodeTypeInputGetMousePosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Input"; }
};