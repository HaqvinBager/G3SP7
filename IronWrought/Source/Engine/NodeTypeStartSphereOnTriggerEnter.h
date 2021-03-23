#pragma once
#include "NodeType.h"
class CNodeTypeStartSphereOnTriggerEnter : public CNodeType
{
public:
	CNodeTypeStartSphereOnTriggerEnter();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Start"; }
	bool IsStartNode() override { return true; }
};