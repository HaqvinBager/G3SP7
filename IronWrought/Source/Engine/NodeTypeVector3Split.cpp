#include "stdafx.h"
#include "NodeTypeVector3Split.h"
#include "NodeInstance.h"

CNodeTypeVector3Split::CNodeTypeVector3Split()
{
	myPins.push_back(SPin("Vec3", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));	//0
	myPins.push_back(SPin("X", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));		//1
	myPins.push_back(SPin("Y", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));		//2
	myPins.push_back(SPin("Z", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));		//3
}

int CNodeTypeVector3Split::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	Vector3 input = NodeData::Get<Vector3>(someData);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[1]);
	memcpy(pins[1].myData, &input.x, sizeof(float));

	DeclareDataOnPinIfNecessary<float>(pins[2]);
	memcpy(pins[2].myData, &input.y, sizeof(float));

	DeclareDataOnPinIfNecessary<float>(pins[3]);
	memcpy(pins[3].myData, &input.z, sizeof(float));

	return -1;
}