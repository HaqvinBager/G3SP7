#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeMathStorage : public CNodeType
{
public:
	CNodeTypeMathStorage();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Storage"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};

