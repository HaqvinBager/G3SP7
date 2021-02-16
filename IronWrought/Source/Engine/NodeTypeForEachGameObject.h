#pragma once
#include "NodeType.h"
#include "NodeTypes.h"
#include <string>

class CNodeTypeForEachGameObject: public CNodeType
{
public:
	CNodeTypeForEachGameObject();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() {
		return "For Each Object";
	}
	std::string GetNodeTypeCategory() override {
		return "Game Object";
	}
};