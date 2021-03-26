#include "stdafx.h"
#include "NodeTypeMathLerp.h"
#include "NodeInstance.h"


CNodeTypeMathLerp::CNodeTypeMathLerp()
{
    myPins.push_back(SPin("A", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
    myPins.push_back(SPin("B", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
    myPins.push_back(SPin("T", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
    myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));
	DeclareDataOnPinIfNecessary<float>(myPins[0], 0.0f);
	DeclareDataOnPinIfNecessary<float>(myPins[1], 0.0f);
	DeclareDataOnPinIfNecessary<float>(myPins[2], 0.0f);
	DeclareDataOnPinIfNecessary<float>(myPins[3], 0.0f);
}

int CNodeTypeMathLerp::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
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
