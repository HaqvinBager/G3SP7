#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVFXPlayVFX : public CNodeType
{
public:
	CNodeTypeVFXPlayVFX();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "VFX"; }
};

