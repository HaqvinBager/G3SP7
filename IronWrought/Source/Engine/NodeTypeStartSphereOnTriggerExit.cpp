#include "stdafx.h"
#include "NodeTypeStartSphereOnTriggerExit.h"

#include "NodeInstance.h"

#include "Engine.h"
#include "Scene.h"

CNodeTypeStartSphereOnTriggerExit::CNodeTypeStartSphereOnTriggerExit()
{
	myPins.push_back(SPin("Radius", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));//0
	myPins.push_back(SPin("Trigger Once", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));//1
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));//2
	
	DeclareDataOnPinIfNecessary<float>(myPins[0]);
	float data = 1.0f;
	memcpy(myPins[0].myData, &data, sizeof(float));

	DeclareDataOnPinIfNecessary<bool>(myPins[1]);
	bool dataB = false;
	memcpy(myPins[1].myData, &dataB, sizeof(bool));
}

int CNodeTypeStartSphereOnTriggerExit::OnEnter(CNodeInstance * aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float radius = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	bool triggerOnce = NodeData::Get<bool>(someData);

	const std::vector<CGameObject*> gameObjects = aTriggeringNodeInstance->GetCurrentGameObject();
	const CTransformComponent& playerTransform = *IRONWROUGHT->GetActiveScene().Player()->myTransform;
	float distanceSquared = Vector3::DistanceSquared(playerTransform.Position(), gameObjects[1]->myTransform->Position());
	float radiusSquared = radius * radius;

	bool trigger = distanceSquared > radiusSquared;
	if (trigger && aTriggeringNodeInstance->myShouldTriggerAgain)
	{	
		aTriggeringNodeInstance->myShouldTriggerAgain = false;
		return 2;
	}
	else
	{
		if (!trigger && !aTriggeringNodeInstance->myShouldTriggerAgain && !triggerOnce)
		{
			aTriggeringNodeInstance->myShouldTriggerAgain = true;
		}
		return -1;
	}
}
