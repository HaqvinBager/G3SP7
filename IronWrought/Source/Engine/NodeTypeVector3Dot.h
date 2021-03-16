#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVector3Dot : public CNodeType
{
public:
	CNodeTypeVector3Dot();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Vector3"; }
};

