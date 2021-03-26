#include "stdafx.h"
#include "NodeTypeMathCos.h"
#include "NodeInstance.h"

CNodeTypeMathCos::CNodeTypeMathCos()
{
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));
	DeclareDataOnPinIfNecessary<float>(myPins[0], 0.0f);
	DeclareDataOnPinIfNecessary<float>(myPins[1], 0.0f);
}

int CNodeTypeMathCos::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float input = NodeData::Get<float>(someData); 

	float result = cosf(input);
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[1]); 
	memcpy(pins[1].myData, &result, sizeof(float)); 

    return -1;
}
