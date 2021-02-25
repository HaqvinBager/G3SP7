#include "stdafx.h"
#include "NodeTypeVariableSetFloat.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableSetFloat::CNodeTypeVariableSetFloat()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::PinTypeInOut_IN));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::PinTypeInOut_OUT));
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));
}

int CNodeTypeVariableSetFloat::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	float input = NodeData::Get<float>(someData);
	CNodeDataManager::Get()->SetData(myNodeDataKey, CNodeDataManager::EDataType::EFloat, &input);

	return 1;
}