#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeAudioPlayResearcherReactionExplosives : public CNodeType
{
public:
	CNodeTypeAudioPlayResearcherReactionExplosives();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Audio"; }
};

