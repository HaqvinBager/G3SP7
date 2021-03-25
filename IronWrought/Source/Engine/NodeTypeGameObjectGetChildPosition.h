#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeGameObjectGetChildPosition : public CNodeType
{
public:
	CNodeTypeGameObjectGetChildPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }
private:
};

