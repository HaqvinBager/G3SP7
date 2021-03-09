#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeAudioPlayRobotPatrolling : public CNodeType
{
public:
	CNodeTypeAudioPlayRobotPatrolling();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Audio"; }
};

