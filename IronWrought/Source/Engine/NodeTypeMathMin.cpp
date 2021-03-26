#include "stdafx.h"
#include "NodeTypeMathMin.h"
#include "NodeInstance.h"

CNodeTypeMathMin::CNodeTypeMathMin()
{
	myPins.push_back(SPin("Val1", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
	myPins.push_back(SPin("Val2", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));
	DeclareDataOnPinIfNecessary<float>(myPins[0], 0.0f);
	DeclareDataOnPinIfNecessary<float>(myPins[1], 0.0f);
	DeclareDataOnPinIfNecessary<float>(myPins[2], 0.0f);
}

int CNodeTypeMathMin::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float input1 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	float input2 = NodeData::Get<float>(someData);

	float temp = (input1 < input2) ? input1 : input2;

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[2]);
	memcpy(pins[2].myData, &temp, sizeof(float));

	return -1;
}

