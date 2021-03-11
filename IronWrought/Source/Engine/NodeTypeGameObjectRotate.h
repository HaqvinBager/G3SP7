#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGameObjectRotate : public CNodeType
{
public:
	CNodeTypeGameObjectRotate();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};