#include "stdafx.h"
#include "NodeTypeFloat.h"
#include "NodeInstance.h"

CNodeTypeFloat::CNodeTypeFloat()
{
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float));
}

int CNodeTypeFloat::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	bool input = NodeData::Get<float>(someData);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[1]);
	memcpy(pins[1].myData, &input, sizeof(float));

	return -1;
}
