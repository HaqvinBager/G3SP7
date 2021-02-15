#include "stdafx.h"
#include "NodeTypeMathDistance.h"
#include "NodeInstance.h"

CNodeTypeMathDistance::CNodeTypeMathDistance()
{
	myPins.push_back(SPin("X1", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float)); // Pin index 0
	myPins.push_back(SPin("Y1", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float)); // Pin index 1
	myPins.push_back(SPin("X2", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float)); // Pin index 0
	myPins.push_back(SPin("Y2", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float)); // Pin index 1
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float)); // Pin index 2
}

int CNodeTypeMathDistance::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float input1 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	float input2 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	float input3 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	float input4 = NodeData::Get<float>(someData);

	float temp = ((input1 - input3) * (input1 - input3)) + ((input2 - input4) * (input2 - input4));

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[4]);
	memcpy(pins[4].myData, &temp, sizeof(float));

	return -1;
}
