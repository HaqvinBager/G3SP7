#include "stdafx.h"
#include "NodeTypeMathMul.h"
#include "NodeInstance.h"

CNodeTypeMathMul::CNodeTypeMathMul()
{
	myPins.push_back(SPin("Val1", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
	myPins.push_back(SPin("Val2", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));
}

int CNodeTypeMathMul::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float input1 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	float input2 = NodeData::Get<float>(someData);

	float temp = input1 * input2;
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[2]);
	memcpy(pins[2].myData, &temp, sizeof(float));
	
	return -1;
}
