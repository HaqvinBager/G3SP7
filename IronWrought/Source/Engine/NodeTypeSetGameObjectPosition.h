#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeSetGameObjectPosition : public CNodeType
{
public:
	CNodeTypeSetGameObjectPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Set Object Position"; }
	std::string GetNodeTypeCategory() override { return "Game Object"; }
};

