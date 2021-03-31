#include "stdafx.h"
#include "NodeTypeGameObjectMoveToPosition.h"
#include "NodeInstance.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "GraphManager.h"
#include "Timer.h"
#include "Scene.h"
#include "Engine.h"

CNodeTypeGameObjectMoveToPosition::CNodeTypeGameObjectMoveToPosition()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFlow));			//0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT));								//1
	myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));		//2
	myPins.push_back(SPin("Is at Pos", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EBool));	//3
	myPins.push_back(SPin("Speed", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));		//4
	DeclareDataOnPinIfNecessary<Vector3>(myPins[2], Vector3(0.f, 0.f, 0.f));
	DeclareDataOnPinIfNecessary<bool>(myPins[3], false);
	DeclareDataOnPinIfNecessary<float>(myPins[4], 0.0f);
}

int CNodeTypeGameObjectMoveToPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(aTriggeringNodeInstance->GraphManager()->GetCurrentBlueprintInstanceID());

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	bool stopMovingX = false;
	bool stopMovingY = false;
	bool stopMovingZ = false;
	bool stopMoving = false;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	Vector3 postion = NodeData::Get<Vector3>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	float speed = NodeData::Get<float>(someData);

	Vector3 currentPosition = gameObject->myTransform->Position();
	Vector3 direction = { (postion.x - currentPosition.x), (postion.y - currentPosition.y), (postion.z - currentPosition.z) };

	if ((direction.x < 0.01f && direction.x > -0.01f))
	{
		stopMovingX = true;
		direction.x = 0.0f;
	}
	if ((direction.y <  0.01f && direction.y > -0.01f))
	{
		stopMovingY = true;
		direction.y = 0.0f;
	}
	if ((direction.z < 0.01f && direction.z > -0.01f))
	{
		stopMovingZ = true;
		direction.z = 0.0f;
	}

	if (stopMovingX && stopMovingY && stopMovingZ)
	{
		stopMoving = true;
	}

	if (!stopMoving)
	{
		direction.Normalize();
		direction *= CTimer::Dt() * speed;

		gameObject->myTransform->Move({ direction.x,  direction.y, direction.z });
	}


	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<bool>(pins[3]);
	memcpy(pins[3].myData, &stopMoving, sizeof(bool));

	return 1;
}
