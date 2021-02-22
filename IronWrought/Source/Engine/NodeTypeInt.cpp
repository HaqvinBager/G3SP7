#include "stdafx.h"
#include "NodeTypeInt.h"
#include "NodeInstance.h"

CNodeTypeInt::CNodeTypeInt()
{
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Int));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Int));
}

int CNodeTypeInt::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	bool input = NodeData::Get<int>(someData);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<int>(pins[1]);
	memcpy(pins[1].myData, &input, sizeof(int));

	return -1;
}
