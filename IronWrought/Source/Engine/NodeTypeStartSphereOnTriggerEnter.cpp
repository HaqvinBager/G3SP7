#include "stdafx.h"
#include "NodeTypeStartSphereOnTriggerEnter.h"

#include "NodeInstance.h"

#include "Engine.h"
#include "Scene.h"

CNodeTypeStartSphereOnTriggerEnter::CNodeTypeStartSphereOnTriggerEnter()
{
	myPins.push_back(SPin("Radius", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));//0
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));							    //1
}

int CNodeTypeStartSphereOnTriggerEnter::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float radius = NodeData::Get<float>(someData);

	std::vector<CGameObject*> gameObjects = aTriggeringNodeInstance->GetCurrentGameObject();
	const CTransformComponent& playerTransform = *IRONWROUGHT->GetActiveScene().Player()->myTransform;
	float distanceSquared = Vector3::DistanceSquared(playerTransform.Position(), gameObjects[0]->myTransform->Position());
	float radiusSquared = radius * radius;
	
	bool trigger = distanceSquared <= radiusSquared;
	aTriggeringNodeInstance->myShouldTriggerAgain = !trigger;
	if (trigger)
		return 1;
	else
		return -1;
}
