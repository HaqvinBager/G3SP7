#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeListTest : public CNodeType
{
public:
	CNodeTypeListTest();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Test"; }
};

