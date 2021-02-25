#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathToRadians : public CNodeType
{
public:
	CNodeTypeMathToRadians();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

