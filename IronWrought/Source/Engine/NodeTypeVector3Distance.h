#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVector3Distance : public CNodeType
{
public:
	CNodeTypeVector3Distance();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override {
		return "Vector3";
	}
};

