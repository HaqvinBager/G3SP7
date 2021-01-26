#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathFloor : public CNodeType
{
public:
	CNodeTypeMathFloor();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Floor"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

