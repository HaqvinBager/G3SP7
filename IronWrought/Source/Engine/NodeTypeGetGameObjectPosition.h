#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGetGameObjectPosition : public CNodeType
{
public:
	CNodeTypeGetGameObjectPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Get Object Position"; }
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};

