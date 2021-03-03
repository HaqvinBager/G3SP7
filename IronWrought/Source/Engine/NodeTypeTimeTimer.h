#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

struct STimerState {
	float myDuration;
	bool myShouldLoop;
	int myTimerUID;
};

class CNodeTypeTimeTimer : public CNodeType
{
public:
	CNodeTypeTimeTimer();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Time"; }

private:
	STimerState UpdateState(CNodeInstance* aTriggeringNodeInstance);

private:
	STimerState myState;
	bool myStateHasChanged = true;
};

