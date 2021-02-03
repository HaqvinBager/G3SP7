#include "stdafx.h"
#include "NodeTypeMathStorage.h"
#include "NodeInstance.h"

CNodeTypeMathStorage::CNodeTypeMathStorage()
{
    myPins.push_back(SPin("Val", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("OUT", SPin::PinTypeInOut::PinTypeInOut_OUT, SPin::PinType::Float));
}

int CNodeTypeMathStorage::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::PinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float input = NodeData::Get<float>(someData);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[1]);
	memcpy(pins[1].myData, &input, sizeof(float));

	return -1;
}
