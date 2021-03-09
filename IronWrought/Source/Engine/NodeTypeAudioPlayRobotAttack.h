#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeAudioPlayRobotAttack : public CNodeType
{
public:
	CNodeTypeAudioPlayRobotAttack();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Audio"; }
};


