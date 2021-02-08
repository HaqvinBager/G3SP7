#include "stdafx.h"
#include "NodeTypeMathLerp.h"
#include "NodeInstance.h"


CNodeTypeMathLerp::CNodeTypeMathLerp()
{
    myPins.push_back(SPin("A", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("B", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("T", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("OUT", SPin::PinTypeInOut::PinTypeInOut_OUT, SPin::PinType::Float));
}

int CNodeTypeMathLerp::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::PinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float a = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	float b = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	float t = NodeData::Get<float>(someData);

	float result = (1 - t) * a + t * b;
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[3]);
	memcpy(pins[3].myData, &result, sizeof(float));

	return -1;
}
