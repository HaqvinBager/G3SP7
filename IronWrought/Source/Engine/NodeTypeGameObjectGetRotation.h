#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGameObjectGetRotation : public CNodeType
{
public:
	CNodeTypeGameObjectGetRotation();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};

