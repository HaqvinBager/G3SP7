#include "stdafx.h"
#include "NodeTypeMathAbs.h"
#include "NodeInstance.h"

CNodeTypeMathAbs::CNodeTypeMathAbs()
{
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float));
}

int CNodeTypeMathAbs::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float input = NodeData::Get<float>(someData);

	float result = abs(input);
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[1]);
	memcpy(pins[1].myData, &result, sizeof(float));

	return -1;
}
