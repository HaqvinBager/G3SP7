#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGameObjectMove : public CNodeType
{
public:
	CNodeTypeGameObjectMove();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};