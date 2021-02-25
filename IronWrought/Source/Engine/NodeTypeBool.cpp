#include "stdafx.h"
#include "NodeTypeBool.h"
#include "NodeInstance.h"

CNodeTypeBool::CNodeTypeBool()
{
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Bool));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Bool));
}

int CNodeTypeBool::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	bool input = NodeData::Get<bool>(someData);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<bool>(pins[1]);
	memcpy(pins[1].myData, &input, sizeof(bool));

	return -1;
}
