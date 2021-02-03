#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathCos : public CNodeType
{
public:
	CNodeTypeMathCos();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Cos"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

