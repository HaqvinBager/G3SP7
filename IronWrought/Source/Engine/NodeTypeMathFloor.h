#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathFloor : public CNodeType
{
public:
	CNodeTypeMathFloor();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

