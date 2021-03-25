#pragma once
#include "NodeType.h"

class CNodeTypeStartSphereOnTriggerStay : public CNodeType
{
public:
	CNodeTypeStartSphereOnTriggerStay();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	bool IsStartNode() override { return true; }
	std::string GetNodeTypeCategory() override { return "Start"; }
};

