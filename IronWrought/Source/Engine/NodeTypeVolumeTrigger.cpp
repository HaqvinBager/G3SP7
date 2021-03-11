#include "stdafx.h"
#include "NodeTypeVolumeTrigger.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "GameObject.h"
#include "GraphNodeCollisionManager.h"

CNodeTypeVolumeTrigger::CNodeTypeVolumeTrigger()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::EPinTypeInOut_IN)); //0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT)); //1
	myPins.push_back(SPin("OnEnter", SPin::EPinTypeInOut::EPinTypeInOut_OUT)); //2
	myPins.push_back(SPin("OnStay", SPin::EPinTypeInOut::EPinTypeInOut_OUT)); //3
	myPins.push_back(SPin("OnExit", SPin::EPinTypeInOut::EPinTypeInOut_OUT)); //4
	myPins.push_back(SPin("Once", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool)); //5
	myPins.push_back(SPin("ListenObject", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EInt)); //6
	myPins.push_back(SPin("TriggerObject", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EInt)); //7
	myPins.push_back(SPin("AffectedObject", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EInt)); //8
	myPins.push_back(SPin("TriggerType", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EString)); //9
	myPins.push_back(SPin("EnteringObjectID", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EInt)); //10
	myPins.push_back(SPin("ExitingObjectID", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EInt)); //11
	myPins.push_back(SPin("ObjectAffected", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EInt)); //12

	myState.myDoOnce = false;
	myState.myListenObjectID = 0;
	myState.myTriggerObjectID = 0;
	myState.myAffectedObjectID = 0;
	myState.myColliderUID = -1001;
	myState.myTriggerTypeToListenTo = "";
}

int CNodeTypeVolumeTrigger::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SVolumeTriggerState currentState = UpdateState(aTriggeringNodeInstance);

	if (myStateHasChanged)
	{
		myStateHasChanged = false;

		CGraphNodeCollisionManager::Get()->AddCollision(
			[aTriggeringNodeInstance, currentState, this](int aListenObjectID) {
				std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
				DeclareDataOnPinIfNecessary<int>(pins[10]);
				memcpy(pins[10].myData, &aListenObjectID, sizeof(int));
				DeclareDataOnPinIfNecessary<int>(pins[12]);
				memcpy(pins[12].myData, &currentState.myAffectedObjectID, sizeof(int));
				aTriggeringNodeInstance->ExitVia(2);
			},

			[aTriggeringNodeInstance, currentState, this](int aListenObjectID) {
				std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
				DeclareDataOnPinIfNecessary<int>(pins[10]);
				memcpy(pins[10].myData, &aListenObjectID, sizeof(int));
				DeclareDataOnPinIfNecessary<int>(pins[12]);
				memcpy(pins[12].myData, &currentState.myAffectedObjectID, sizeof(int));
				aTriggeringNodeInstance->ExitVia(3);
			},

			[aTriggeringNodeInstance, currentState, this](int aListenObjectID) {
			std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
			DeclareDataOnPinIfNecessary<int>(pins[11]);
			memcpy(pins[11].myData, &aListenObjectID, sizeof(int));
			DeclareDataOnPinIfNecessary<int>(pins[12]);
			memcpy(pins[12].myData, &currentState.myAffectedObjectID, sizeof(int));
			aTriggeringNodeInstance->ExitVia(4);
			},

			currentState.myTriggerTypeToListenTo,
			currentState.myListenObjectID,
			currentState.myTriggerObjectID,
			currentState.myColliderUID,
			currentState.myDoOnce
			);
	}

	return 1;
}

SVolumeTriggerState CNodeTypeVolumeTrigger::UpdateState(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 5, outType, someData, outSize);
	bool doOnce = NodeData::Get<bool>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 6, outType, someData, outSize);
	int listenObjectID = NodeData::Get<int>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 7, outType, someData, outSize);
	int triggerObjectID = NodeData::Get<int>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 8, outType, someData, outSize);
	int affectedObjectID = NodeData::Get<int>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 9, outType, someData, outSize);
	std::string triggerType = static_cast<char*>(someData);

	if (
		myState.myDoOnce != doOnce ||
		myState.myListenObjectID != listenObjectID ||
		myState.myTriggerObjectID != triggerObjectID ||
		myState.myAffectedObjectID != affectedObjectID ||
		myState.myTriggerTypeToListenTo != triggerType
		)
	{
		myStateHasChanged = true;

		CGraphNodeCollisionManager::Get()->RemoveColliderWithID(myState.myColliderUID);

		myState.myDoOnce = doOnce;
		myState.myListenObjectID = listenObjectID;
		myState.myTriggerObjectID = triggerObjectID;
		myState.myAffectedObjectID = affectedObjectID;
		myState.myColliderUID = CGraphNodeCollisionManager::Get()->RequestUID();
		myState.myTriggerTypeToListenTo = triggerType;
	}

	return myState;
}
