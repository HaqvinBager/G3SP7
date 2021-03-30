#include "stdafx.h"
#include "NodeTypeVector3Sub.h"
#include "NodeInstance.h"

CNodeTypeVector3Sub::CNodeTypeVector3Sub()
{
	myPins.push_back(SPin("Val1", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3)); // Pin index 0
	myPins.push_back(SPin("Val2", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3)); // Pin index 1
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3)); // Pin index 2
	DeclareDataOnPinIfNecessary<Vector3>(myPins[0], Vector3(1.0f,1.0f,1.0f));
	DeclareDataOnPinIfNecessary<Vector3>(myPins[1], Vector3(1.0f,1.0f,1.0f));
	DeclareDataOnPinIfNecessary<Vector3>(myPins[2], Vector3(1.0f,1.0f,1.0f));
}

int CNodeTypeVector3Sub::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	Vector3 input1 = NodeData::Get<Vector3>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	Vector3 input2 = NodeData::Get<Vector3>(someData);

	Vector3 temp = input1 - input2;
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[2]);
	memcpy(pins[2].myData, &temp, sizeof(Vector3));

	return -1;
}
