#pragma once
#include "NodeType.h"
class CNodeTypeVector3Sub : public CNodeType
{
public:
	CNodeTypeVector3Sub();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Vector3"; }
};

