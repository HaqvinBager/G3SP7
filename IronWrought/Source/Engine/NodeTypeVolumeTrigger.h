#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

struct SVolumeTriggerState {
	std::string myTriggerTypeToListenTo;
	int myListenObjectID;
	int myTriggerObjectID;
	int myAffectedObjectID;
	int myColliderUID;
	bool myDoOnce;
};

class CNodeTypeVolumeTrigger: public CNodeType
{
public:
	CNodeTypeVolumeTrigger();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() {
		return "Volume Trigger";
	}
	std::string GetNodeTypeCategory() override {
		return "Triggers";
	}

private:
	SVolumeTriggerState UpdateState(CNodeInstance* aTriggeringNodeInstance);

private:
	SVolumeTriggerState myState;
	
	bool myCollided = false;
	bool myStateHasChanged = true;
	int myExitingID = 0;
};

