#include "stdafx.h"
#include "NodeTypeVector3Dot.h"
#include "NodeInstance.h"

CNodeTypeVector3Dot::CNodeTypeVector3Dot()
{
	myPins.push_back(SPin("Pos1", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));
	myPins.push_back(SPin("Pos2", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));
}

int CNodeTypeVector3Dot::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	Vector3 position1 = NodeData::Get<Vector3>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	Vector3 position2 = NodeData::Get<Vector3>(someData);

	float result = position1.Dot(position2);/*position1.x * position2.x + position1.y * position2.y + position1.z * position2.z;*/
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[6]);
	memcpy(pins[6].myData, &result, sizeof(float));

	return -1;
}
