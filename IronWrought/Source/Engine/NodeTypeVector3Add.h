#pragma once
#include "NodeType.h"
class CNodeTypeVector3Add : public CNodeType
{
public:
	CNodeTypeVector3Add();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Vector3"; }
};

