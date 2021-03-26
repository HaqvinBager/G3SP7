#include "stdafx.h"
#include "NodeTypeTimeTimer.h"
#include "NodeInstance.h"
#include <iostream>
#include "GraphNodeTimerManager.h"
#include "NodeInstance.h"

CNodeTypeTimeTimer::CNodeTypeTimeTimer()
{

	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFlow)); //0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT)); //1
	myPins.push_back(SPin("Sec ", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat)); //2
	myPins.push_back(SPin("Looping", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool)); //3
	myPins.push_back(SPin("OnRing", SPin::EPinTypeInOut::EPinTypeInOut_OUT)); //4

	DeclareDataOnPinIfNecessary<float>(myPins[2], 0.0f);
	DeclareDataOnPinIfNecessary<bool>(myPins[3], false);

	myState.myDuration = 0.0f;
	myState.myShouldLoop = false;
	myState.myTimerUID = -1001;
}

int CNodeTypeTimeTimer::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	STimerState currentState = UpdateState(aTriggeringNodeInstance);

	if (myStateHasChanged)
	{
		myStateHasChanged = false;

		CGraphNodeTimerManager::Get()->AddTimer(
			[aTriggeringNodeInstance] 
			{ 
				aTriggeringNodeInstance->ExitVia(4); 
			},
				myState.myDuration,
				myState.myTimerUID,
				myState.myShouldLoop
			);
	}

	return 1;
}

STimerState CNodeTypeTimeTimer::UpdateState(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	float duration = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	bool shouldLoop = NodeData::Get<bool>(someData);

	if (
		myState.myDuration != duration ||
		myState.myShouldLoop != shouldLoop
		)
	{
		myStateHasChanged = true;
		
		CGraphNodeTimerManager::Get()->RemoveTimerWithID(myState.myTimerUID);
		
		myState.myDuration = duration;
		myState.myTimerUID = CGraphNodeTimerManager::Get()->RequestUID();
		myState.myShouldLoop = shouldLoop;
	}

	return myState;
}
