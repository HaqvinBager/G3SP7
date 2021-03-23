#pragma once
#include "NodeType.h"
class CNodeTypePlayerGetPosition : public CNodeType
{
public:
	CNodeTypePlayerGetPosition();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Player"; }
};

