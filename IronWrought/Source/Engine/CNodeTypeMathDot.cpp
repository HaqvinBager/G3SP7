#include "stdafx.h"
#include "CNodeTypeMathDot.h"
#include "CNodeInstance.h"

CNodeTypeMathDot::CNodeTypeMathDot()
{
    myPins.push_back(SPin("X1", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("Y1", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("Z1", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("X2", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("Y2", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("Z2", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("OUT", SPin::PinTypeInOut::PinTypeInOut_OUT, SPin::PinType::Float));
}

int CNodeTypeMathDot::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::PinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float x1 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	float y1 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	float z1 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	float x2 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	float y2 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 5, outType, someData, outSize);
	float z2 = NodeData::Get<float>(someData);

	float result = x1*x2 + y1*y2 + z1*z2;
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[6]);
	memcpy(pins[6].myData, &result, sizeof(float));

	return -1;
}
