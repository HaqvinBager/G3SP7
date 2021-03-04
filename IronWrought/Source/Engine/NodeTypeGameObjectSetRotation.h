#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGameObjectSetRotation : public CNodeType
{
public:
	CNodeTypeGameObjectSetRotation();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};

