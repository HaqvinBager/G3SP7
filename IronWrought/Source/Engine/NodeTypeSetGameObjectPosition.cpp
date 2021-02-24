#include "stdafx.h"
#include "NodeTypeSetGameObjectPosition.h"
#include "NodeInstance.h"
#include "GameObject.h"
#include "TransformComponent.h"

CNodeTypeSetGameObjectPosition::CNodeTypeSetGameObjectPosition()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Flow));	//0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT));						//1
    myPins.push_back(SPin("X", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));	//2
    myPins.push_back(SPin("Y", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));	//3
    myPins.push_back(SPin("Z", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));	//4
}

int CNodeTypeSetGameObjectPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject();

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	float x = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	float y = NodeData::Get<float>(someData);
	
	GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	float z = NodeData::Get<float>(someData);

	Vector3 newPosition = { x, y, z };
	gameObject->myTransform->Position(newPosition);

	return 1;
}
