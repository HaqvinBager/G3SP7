#include "stdafx.h"
#include "NodeTypeUtilitiesBranching.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeUtilitiesBranching::CNodeTypeUtilitiesBranching()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));								//0
	myPins.push_back(SPin("True", SPin::EPinTypeInOut::EPinTypeInOut_OUT));							//1
	myPins.push_back(SPin("False", SPin::EPinTypeInOut::EPinTypeInOut_OUT));							//2
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));		//3
}

int CNodeTypeUtilitiesBranching::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	bool input = NodeData::Get<bool>(someData);

	if (input)
		return 1;
	else
		return 2;
}