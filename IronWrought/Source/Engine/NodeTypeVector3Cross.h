#pragma once
#include "NodeType.h"

class CNodeTypeVector3Cross : public CNodeType
{
public:
	CNodeTypeVector3Cross();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Vector3"; }
};

