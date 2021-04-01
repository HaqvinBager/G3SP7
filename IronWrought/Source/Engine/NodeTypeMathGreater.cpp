#include "stdafx.h"
#include "NodeTypeMathGreater.h"
#include "NodeInstance.h"

CNodeTypeMathGreater::CNodeTypeMathGreater()
{
	myPins.push_back(SPin("Val1", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//0
	myPins.push_back(SPin("Val2", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//1
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EBool));	//2
}

int CNodeTypeMathGreater::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float input1 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	float input2 = NodeData::Get<float>(someData);

	bool result = (input1 > input2) ? true : false;
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<bool>(pins[2]);
	memcpy(pins[2].myData, &result, sizeof(bool));

	return -1;
}
