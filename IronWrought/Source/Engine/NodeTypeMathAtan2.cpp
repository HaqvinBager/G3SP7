#include "stdafx.h"
#include "NodeTypeMathAtan2.h"
#include "NodeInstance.h"

CNodeTypeMathAtan2::CNodeTypeMathAtan2()
{
    myPins.push_back(SPin("X", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("Y", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("OUT", SPin::PinTypeInOut::PinTypeInOut_OUT, SPin::PinType::Float));
}

int CNodeTypeMathAtan2::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::PinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float x = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	float y = NodeData::Get<float>(someData);

	float result = atan2(y, x);
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[2]);
	memcpy(pins[2].myData, &result, sizeof(float));

	return -1;
}
