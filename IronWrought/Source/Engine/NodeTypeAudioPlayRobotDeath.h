#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeAudioPlayRobotDeath : public CNodeType
{
public:
	CNodeTypeAudioPlayRobotDeath();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Audio"; }
};

