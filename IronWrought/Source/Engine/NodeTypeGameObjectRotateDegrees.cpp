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
	int currentID = gameObject->InstanceID();
	const auto searchResult = myRotations.find(gameObject->InstanceID());

	if (myRotations.end() == searchResult)
	{
		myRotations[currentID] = gameObject->myTransform->Rotation();
		myIsFinished[currentID] = false;
	}

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();

	GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	float t = NodeData::Get<float>(someData) * CTimer::Dt() * (3.14159265f / 180.0f);
	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	Vector3 input = NodeData::Get<Vector3>(someData) * (3.14159265f / 180.0f);

	if (static_cast<int>(myInput.x) != static_cast<int>(input.x))
	{
		myInput.x = input.x;
		myIsFinished[currentID] = false;
	}
	if (static_cast<int>(myInput.y) != static_cast<int>(input.y))
	{
		myInput.y = input.y;
		myIsFinished[currentID] = false;
	}
	if (static_cast<int>(myInput.z) != static_cast<int>(input.z))
	{
		myInput.z = input.z;
		myIsFinished[currentID] = false;
	}


	if (!myIsFinished[currentID])
	{
		Quaternion a = gameObject->myTransform->Rotation();
		Quaternion b = Quaternion::CreateFromYawPitchRoll(input.y, input.x, input.z) * myRotations[currentID];
		if (((b.x - a.x) < 0.01f && (b.x - a.x) > -0.01f) || ((b.x + a.x) < 0.01f && (b.x + a.x) > -0.01f))
		{
			if (((b.y - a.y) < 0.01f && (b.y - a.y) > -0.01f) || ((b.y + a.y) < 0.01f && (b.y + a.y) > -0.01f))
			{
				if (((b.z - a.z) < 0.01f && (b.z - a.z) > -0.01f) || ((b.z + a.z) < 0.01f && (b.z + a.z) > -0.01f))
				{
					if (((b.w - a.w) < 0.01f && (b.w - a.w) > -0.01f) || ((b.w + a.w) < 0.01f && (b.w + a.w) > -0.01f))
					{
						myIsFinished[currentID] = true;
						DeclareDataOnPinIfNecessary<bool>(pins[3]);
						memcpy(pins[3].myData, &myIsFinished[currentID], sizeof(bool));

						return 1;
					}
				}
			}
		}

		Quaternion r;
		Quaternion::Lerp(a, b, t, r);
		gameObject->myTransform->Rotation(r);

		DeclareDataOnPinIfNecessary<bool>(pins[3]);
		memcpy(pins[3].myData, &myIsFinished[currentID], sizeof(bool));

		return 1;
	}
	else
	{
		DeclareDataOnPinIfNecessary<bool>(pins[3]);
		memcpy(pins[3].myData, &myIsFinished[currentID], sizeof(bool));

		return 1;
	}
}
