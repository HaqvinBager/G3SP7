#include "stdafx.h"
#include "NodeTypeGameObjectMove.h"
#include "NodeInstance.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"

CNodeTypeGameObjectMove::CNodeTypeGameObjectMove()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Flow));		//0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT));							//1
	myPins.push_back(SPin("X Speed", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));	//2
	myPins.push_back(SPin("Y Speed", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));	//3
	myPins.push_back(SPin("Z Speed", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));	//4
}

int CNodeTypeGameObjectMove::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject()[0];

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	float x = NodeData::Get<float>(someData) * CTimer::Dt();

	GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	float y = NodeData::Get<float>(someData) * CTimer::Dt();;

	GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	float z = NodeData::Get<float>(someData) * CTimer::Dt();;

	Vector3 currentPosition = gameObject->myTransform->Position();

	Vector3 newPosition = { currentPosition.x + x, currentPosition.y + y, currentPosition.z + z };
	gameObject->myTransform->Position(newPosition);

	return 1;
}
