#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeAudioPlayRobotIdle : public CNodeType
{
public:
	CNodeTypeAudioPlayRobotIdle();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Audio"; }
};

