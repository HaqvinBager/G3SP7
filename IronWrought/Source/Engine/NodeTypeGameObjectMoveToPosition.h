#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGameObjectMoveToPosition : public CNodeType
{
public:
	CNodeTypeGameObjectMoveToPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};