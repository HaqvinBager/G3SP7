#include "stdafx.h"
#include "NodeTypeVariableSetInt.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableSetInt::CNodeTypeVariableSetInt()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::PinTypeInOut_IN));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::PinTypeInOut_OUT));
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Int));
}

int CNodeTypeVariableSetInt::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	int input = NodeData::Get<int>(someData);
	CNodeDataManager::Get()->SetData(myNodeDataKey, CNodeDataManager::EDataType::EInt, &input);

	return 1;
}
