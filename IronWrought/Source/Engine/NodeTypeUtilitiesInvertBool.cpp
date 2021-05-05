#include "stdafx.h"
#include "NodeTypeUtilitiesInvertBool.h"
#include "NodeInstance.h"

CNodeTypeUtilitiesInvertBool::CNodeTypeUtilitiesInvertBool()
{
    myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));								//0
    myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));								//1
    myPins.push_back(SPin("Val", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));		//2
    myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EBool));		//3
}

int CNodeTypeUtilitiesInvertBool::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	bool input = !NodeData::Get<bool>(someData);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<bool>(pins[3]);
	memcpy(pins[3].myData, &input, sizeof(bool));

	return 1;
}
