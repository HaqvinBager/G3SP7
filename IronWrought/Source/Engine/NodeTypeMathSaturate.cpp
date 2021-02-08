#include "stdafx.h"
#include "NodeTypeMathSaturate.h"
#include "NodeInstance.h"

CNodeTypeMathSaturate::CNodeTypeMathSaturate()
{
    myPins.push_back(SPin("Val", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("OUT", SPin::PinTypeInOut::PinTypeInOut_OUT, SPin::PinType::Float));
}

int CNodeTypeMathSaturate::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::PinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float input = NodeData::Get<float>(someData);

	float result = input;
	
	if (result < 0.0f)
		result = 0.0f;

	if (result > 1.0f)
		result = 1.0f;

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[1]);
	memcpy(pins[1].myData, &result, sizeof(float));

	return -1;
}
