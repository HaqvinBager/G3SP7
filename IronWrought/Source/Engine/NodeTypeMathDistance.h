#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathDistance: public CNodeType
{
public:
	CNodeTypeMathDistance();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override {
		return "Math";
	}
};

