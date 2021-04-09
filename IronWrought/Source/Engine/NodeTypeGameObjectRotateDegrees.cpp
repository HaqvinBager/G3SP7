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
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFlow));		//0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT));							//1
	myPins.push_back(SPin("Rot", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));	//2
	myPins.push_back(SPin("Speed", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//3
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

	GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	float t = NodeData::Get<float>(someData) * CTimer::Dt() * (3.14159265f / 180.0f);
	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	Vector3 input = NodeData::Get<Vector3>(someData) * (3.14159265f / 180.0f);

	Quaternion a = gameObject->myTransform->Rotation();
	Quaternion b = Quaternion::CreateFromYawPitchRoll(input.y, input.x, input.z) * myRotations[gameObject->InstanceID()];
	Quaternion r;
	Quaternion::Lerp(a, b, t, r);
	gameObject->myTransform->Rotation(r);

	return 1;
}
