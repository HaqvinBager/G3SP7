#include "stdafx.h"
#include "NodeTypeGameObjectSetPosition.h"
#include "NodeInstance.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "GraphManager.h"
#include "Scene.h"
#include "Engine.h"

CNodeTypeGameObjectSetPosition::CNodeTypeGameObjectSetPosition()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFlow));	//0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT));						//1
    myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));	//2
    //myPins.push_back(SPin("Y", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//3
    //myPins.push_back(SPin("Z", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//4
}

int CNodeTypeGameObjectSetPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(aTriggeringNodeInstance->GraphManager()->GetCurrentBlueprintInstanceID());

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	Vector3 newPosition = NodeData::Get<Vector3>(someData);

	//GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	//float y = NodeData::Get<float>(someData);
	//
	//GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	//float z = NodeData::Get<float>(someData);

	//Vector3 newPosition = { x, y, z };
	gameObject->myTransform->Position(newPosition);

	return 1;
}
