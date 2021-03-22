#include "stdafx.h"
#include "NodeTypeStartSphereOnTriggerStay.h"
#include "NodeType.h"
#include "NodeInstance.h"


CNodeTypeStartSphereOnTriggerStay::CNodeTypeStartSphereOnTriggerStay()
{
	myPins.push_back(SPin("Radius", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));
}

int CNodeTypeStartSphereOnTriggerStay::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	std::vector<CGameObject*> gameObject = aTriggeringNodeInstance->GetCurrentGameObject();
	Vector3 position = gameObject[1]->myTransform->Position();
	Vector3 playerPosition;

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float radius = NodeData::Get<float>(someData);
	radius *= radius;
	float distance = Vector3::DistanceSquared(playerPosition, position);
		aTriggeringNodeInstance->myShouldTriggerAgain = true;

	if (distance < radius)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}
