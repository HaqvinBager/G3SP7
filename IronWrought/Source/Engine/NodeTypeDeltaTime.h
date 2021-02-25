#pragma once
#include "NodeType.h"
class CNodeTypeDeltaTime : public CNodeType
{
public:
	CNodeTypeDeltaTime();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Delta Time"; }
	std::string GetNodeTypeCategory() override { return "Timer"; }
};
