#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeAudioPlayResearcherEvent : public CNodeType
{
public:
	CNodeTypeAudioPlayResearcherEvent();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Audio"; }

private:
	int myMaxValue = 1;
};


