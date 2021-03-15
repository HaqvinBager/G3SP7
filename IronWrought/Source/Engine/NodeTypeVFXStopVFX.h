#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVFXStopVFX : public CNodeType
{
public:
	CNodeTypeVFXStopVFX();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "VFX"; }
};

