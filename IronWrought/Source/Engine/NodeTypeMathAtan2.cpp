#include "stdafx.h"
#include "NodeTypeMathAtan2.h"
#include "NodeInstance.h"

CNodeTypeMathAtan2::CNodeTypeMathAtan2()
{
    myPins.push_back(SPin("X", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
    myPins.push_back(SPin("Y", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
    myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));
	DeclareDataOnPinIfNecessary<float>(myPins[0], 0.0f);
	DeclareDataOnPinIfNecessary<float>(myPins[1], 0.0f);
	DeclareDataOnPinIfNecessary<float>(myPins[2], 0.0f);
}

int CNodeTypeMathAtan2::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
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
