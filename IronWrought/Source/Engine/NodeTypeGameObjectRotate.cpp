#include "stdafx.h"
#include "NodeTypeGameObjectRotate.h"
#include "NodeInstance.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"
#include "Scene.h"
#include "Engine.h"
#include "GraphManager.h"

CNodeTypeGameObjectRotate::CNodeTypeGameObjectRotate()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFlow));		//0
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT));							//1
	myPins.push_back(SPin("Rot", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));	//2
	//myPins.push_back(SPin("Yaw", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//3
	//myPins.push_back(SPin("Roll", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//4
}

int CNodeTypeGameObjectRotate::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject();

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	Vector3 newRotation = NodeData::Get<Vector3>(someData) * CTimer::Dt() * 3.14159265f / 180.0f;

	//GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	//float y = NodeData::Get<float>(someData) * CTimer::Dt() * 3.14159265f / 180.0f;

	//GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	//float z = NodeData::Get<float>(someData) * CTimer::Dt() * 3.14159265f / 180.0f;

	DirectX::SimpleMath::Quaternion currentRotation = gameObject->myTransform->Rotation();
	DirectX::SimpleMath::Quaternion rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(newRotation.y, newRotation.x, newRotation.z);
	rotation *= currentRotation;
	gameObject->myTransform->Rotation(rotation);

	return 1;
}
