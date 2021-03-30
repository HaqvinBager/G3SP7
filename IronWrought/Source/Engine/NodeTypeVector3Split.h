#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVector3Split : public CNodeType
{
public:
	CNodeTypeVector3Split();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Vector3"; }
};

