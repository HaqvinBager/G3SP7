#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathLerp : public CNodeType
{
public:
	CNodeTypeMathLerp();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

