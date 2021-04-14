#include "stdafx.h"
#include "NodeTypeGameObjectRotateDegrees.h"
#include "NodeInstance.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"
#include "Scene.h"
#include "Engine.h"
#include "GraphManager.h"

CNodeTypeGameObjectRotateDegrees::CNodeTypeGameObjectRotateDegrees()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFlow));			//0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT));								//1
	myPins.push_back(SPin("Rot", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));		//2
	myPins.push_back(SPin("Finished", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EBool));	//3
	myPins.push_back(SPin("Speed", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));		//4
}

int CNodeTypeGameObjectRotateDegrees::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(aTriggeringNodeInstance->GraphManager()->GetCurrentBlueprintInstanceID());

	const auto searchResult = myRotations.find(gameObject->InstanceID());

	if (myRotations.end() == searchResult)
	{
		myRotations[gameObject->InstanceID()] = gameObject->myTransform->Rotation();
	}

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	float t = NodeData::Get<float>(someData) * CTimer::Dt() * (3.14159265f / 180.0f);
	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	Vector3 input = NodeData::Get<Vector3>(someData) * (3.14159265f / 180.0f);

	Quaternion a = gameObject->myTransform->Rotation();
	Quaternion b = Quaternion::CreateFromYawPitchRoll(input.y, input.x, input.z) * myRotations[gameObject->InstanceID()];
	Quaternion r;
	Quaternion::Lerp(a, b, t, r);

	bool stopMovingX = false;
	bool stopMovingY = false;
	bool stopMovingZ = false;
	bool stopMovingW = false;
	bool stopMoving = false;

	if (((b.x - r.x) < 0.01f && (b.x - r.x) > -0.01f))
	{
		stopMovingX = true;
	}
	if (((b.y - r.y) < 0.01f && (b.y - r.y) > -0.01f))
	{
		stopMovingY = true;
	}
	if (((b.z - r.z) < 0.01f && (b.z - r.z) > -0.01f))
	{
		stopMovingZ = true;
	}
	if (((b.w - r.w) < 0.01f && (b.w - r.w) > -0.01f))
	{
		stopMovingW = true;
	}

	if (stopMovingX && stopMovingY && stopMovingZ && stopMovingW)
	{
		stopMoving = true;
	}

	if (!stopMoving)
		gameObject->myTransform->Rotation(r);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<bool>(pins[3]);
	memcpy(pins[3].myData, &stopMoving, sizeof(bool));

	return 1;
}
