#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

struct STimerState {
	float myDuration;
	bool myShouldLoop;
	int myTimerUID;
};

class CNodeTypeTimer: public CNodeType
{
public:
	CNodeTypeTimer();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() {
		return "Timer";
	}
	std::string GetNodeTypeCategory() override {
		return "Timers";
	}

private:
	STimerState UpdateState(CNodeInstance* aTriggeringNodeInstance);

private:
	STimerState myState;
	bool myStateHasChanged = true;
};

