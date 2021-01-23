#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathMax: public CNodeType
{
public:
	CNodeTypeMathMax();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() {
		return "Max";
	}
	std::string GetNodeTypeCategory() override {
		return "Math";
	}
};

