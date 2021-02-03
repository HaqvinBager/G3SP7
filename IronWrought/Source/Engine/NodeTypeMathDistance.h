#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathDistance: public CNodeType
{
public:
	CNodeTypeMathDistance();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() {
		return "Distance";
	}
	std::string GetNodeTypeCategory() override {
		return "Math";
	}
};

