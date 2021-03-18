#include "stdafx.h"
#include "NodeTypeVector3Distance.h"
#include "NodeInstance.h"

CNodeTypeVector3Distance::CNodeTypeVector3Distance()
{
	myPins.push_back(SPin("Pos1", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3)); // Pin index 0
	myPins.push_back(SPin("Pos2", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3)); // Pin index 0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat)); // Pin index 2
}

int CNodeTypeVector3Distance::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	Vector3 input1 = NodeData::Get<Vector3>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	Vector3 input2 = NodeData::Get<Vector3>(someData);
	float temp = Vector3::Distance(input1, input2);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[2]);
	memcpy(pins[2].myData, &temp, sizeof(float));

	return -1;
}
