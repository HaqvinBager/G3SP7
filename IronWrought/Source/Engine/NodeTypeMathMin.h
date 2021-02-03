#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathMin : public CNodeType
{
public:
	CNodeTypeMathMin();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() {
		return "Min";
	}
	std::string GetNodeTypeCategory() override {
		return "Math";
	}
};

