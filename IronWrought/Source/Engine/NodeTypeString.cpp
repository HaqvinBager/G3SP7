#include "stdafx.h"
#include "NodeTypeString.h"
#include "NodeInstance.h"

CNodeTypeString::CNodeTypeString()
{
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::String));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::String));
}

int CNodeTypeString::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	std::string input = static_cast<char *>(someData);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<char *>(pins[1]);
	memcpy(pins[1].myData, &input, sizeof(char*));

	return -1;
}
