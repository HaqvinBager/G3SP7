#include "stdafx.h"
#include "NodeTypeGameObjectMove.h"
#include "NodeInstance.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"
#include "GraphManager.h"
#include "Scene.h"
#include "Engine.h"

CNodeTypeGameObjectMove::CNodeTypeGameObjectMove()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFlow));		//0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT));							//1
	myPins.push_back(SPin("Speed", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));	//2
	//myPins.push_back(SPin("Y Speed", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//3
	//myPins.push_back(SPin("Z Speed", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//4
	DeclareDataOnPinIfNecessary<Vector3>(myPins[2], Vector3(0.0f, 0.0f, 0.0f));
}

int CNodeTypeGameObjectMove::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(aTriggeringNodeInstance->GraphManager()->GetCurrentBlueprintInstanceID());

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	Vector3 newPosition = NodeData::Get<Vector3>(someData) * CTimer::Dt();

	//GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	//float y = NodeData::Get<float>(someData) * CTimer::Dt();

	//GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	//float z = NodeData::Get<float>(someData) * CTimer::Dt();

	gameObject->myTransform->MoveLocal(newPosition);

	return 1;
}
