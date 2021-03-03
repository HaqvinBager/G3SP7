#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGameObjectGetPosition : public CNodeType
{
public:
	CNodeTypeGameObjectGetPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};

