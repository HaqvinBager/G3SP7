#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGameObjectSetPosition : public CNodeType
{
public:
	CNodeTypeGameObjectSetPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};

