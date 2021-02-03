#pragma once
#include "NodeType.h"
#include "NodeTypes.h"
#include <string>

class CNodeTypeGetMousePosition : public CNodeType
{
public:
	CNodeTypeGetMousePosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() {
		return "Mouse Position";
	}
	std::string GetNodeTypeCategory() override {
		return "Input";
	}
};