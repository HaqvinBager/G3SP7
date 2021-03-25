#include "stdafx.h"
#include "NodeTypeVector3Join.h"
#include "NodeInstance.h"

CNodeTypeVector3Join::CNodeTypeVector3Join()
{
	myPins.push_back(SPin("X", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));			//0
	myPins.push_back(SPin("Y", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));			//1
	myPins.push_back(SPin("Z", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));			//2
	myPins.push_back(SPin("Vec3", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3));	//3
}

int CNodeTypeVector3Join::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	Vector3 input;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	input.x = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	input.y = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	input.z = NodeData::Get<float>(someData);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[3]);
	memcpy(pins[3].myData, &input, sizeof(Vector3));

	return -1;
}
