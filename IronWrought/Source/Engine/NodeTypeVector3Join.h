#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVector3Join : public CNodeType
{
public:
	CNodeTypeVector3Join();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Vector3"; }
};

