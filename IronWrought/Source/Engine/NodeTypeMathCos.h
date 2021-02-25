#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathCos : public CNodeType
{
public:
	CNodeTypeMathCos();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Math"; }
};

