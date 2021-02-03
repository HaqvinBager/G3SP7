#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathAtan2 : public CNodeType
{
public:
	CNodeTypeMathAtan2();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Atan2"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

