#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeAddForce : public CNodeType
{
	public:
		CNodeTypeAddForce();
		int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
		std::string GetNodeTypeCategory() override { return "Physics"; }
	
};

