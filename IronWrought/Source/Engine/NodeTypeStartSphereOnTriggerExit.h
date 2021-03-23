#pragma once
#include "NodeType.h"
class CNodeTypeStartSphereOnTriggerExit : public CNodeType
{
public:
	CNodeTypeStartSphereOnTriggerExit();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Start"; }
	bool IsStartNode() override { return true; }
};

