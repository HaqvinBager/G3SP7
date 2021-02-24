#include "stdafx.h"
#include "NodeTypeStart.h"
#include <iostream>
#include "NodeType.h"
#include "NodeInstance.h"


CNodeTypeStart::CNodeTypeStart()
{
	myPins.push_back(SPin("Updating", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Bool));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::PinTypeInOut_OUT));
}

int CNodeTypeStart::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	aTriggeringNodeInstance->myShouldTriggerAgain = NodeData::Get<bool>(someData);

	return 1;
}
