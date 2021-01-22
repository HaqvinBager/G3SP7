#include "stdafx.h"
#include "CNodeTypeStart.h"
#include <iostream>
#include "CNodeType.h"
#include "CNodeInstance.h"


CNodeTypeStart::CNodeTypeStart()
{
	myPins.push_back(SPin("Updating", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Bool));
	myPins.push_back(SPin("", SPin::PinTypeInOut::PinTypeInOut_OUT));
}

int CNodeTypeStart::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	SPin::PinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	aTriggeringNodeInstance->myShouldTriggerAgain = NodeData::Get<bool>(someData);
	return 1;
}
