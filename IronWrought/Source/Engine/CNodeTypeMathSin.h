#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathSin : public CNodeType
{
public:
	CNodeTypeMathSin();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Sin"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

