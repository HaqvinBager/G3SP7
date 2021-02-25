#pragma once
#include "NodeType.h"
class CNodeTypeTimeDeltaTotal : public CNodeType
{
public:
	CNodeTypeTimeDeltaTotal();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Time"; }
};
