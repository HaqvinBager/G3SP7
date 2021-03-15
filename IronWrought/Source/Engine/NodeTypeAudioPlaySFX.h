#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeAudioPlaySFX : public CNodeType
{
public:
	CNodeTypeAudioPlaySFX();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Audio"; }
};

