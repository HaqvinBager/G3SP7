#include "stdafx.h"
#include "NodeTypeStartDefault.h"
#include <iostream>
#include "NodeType.h"
#include "NodeInstance.h"


CNodeTypeStartDefault::CNodeTypeStartDefault()
{
	myPins.push_back(SPin("Updating", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));
	DeclareDataOnPinIfNecessary<bool>(myPins[0], false);

}

int CNodeTypeStartDefault::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	aTriggeringNodeInstance->myShouldTriggerAgain = NodeData::Get<bool>(someData);

	return 1;
}
