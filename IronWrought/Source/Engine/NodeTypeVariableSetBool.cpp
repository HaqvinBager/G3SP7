#include "stdafx.h"
#include "NodeTypeVariableSetBool.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableSetBool::CNodeTypeVariableSetBool()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::PinTypeInOut_IN));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::PinTypeInOut_OUT));
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Bool));
}

int CNodeTypeVariableSetBool::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	bool input = NodeData::Get<bool>(someData);
	CNodeDataManager::Get()->SetData(myNodeDataKey, CNodeDataManager::EDataType::EBool, &input);

	return 1;
}