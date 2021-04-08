#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGameObjectRotateDegrees : public CNodeType
{
public:
	CNodeTypeGameObjectRotateDegrees();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};

