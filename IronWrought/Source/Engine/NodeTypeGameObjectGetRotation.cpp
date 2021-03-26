#include "stdafx.h"
#include "NodeTypeGameObjectGetRotation.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "GameObject.h"
#include "GraphManager.h"

CNodeTypeGameObjectGetRotation::CNodeTypeGameObjectGetRotation()
{
	myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3)); //0
	//myPins.push_back(SPin("Y", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat)); //1
	//myPins.push_back(SPin("Z", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat)); //2
	DeclareDataOnPinIfNecessary<Vector3>(myPins[0], Vector3(0.f, 0.f, 0.f));
}

int CNodeTypeGameObjectGetRotation::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject()[1];
	Vector3 rotation = gameObject->myTransform->Position();

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[0]);
	memcpy(pins[0].myData, &rotation, sizeof(Vector3));
	//DeclareDataOnPinIfNecessary<float>(pins[1]);
	//memcpy(pins[1].myData, &rotation.y, sizeof(float));
	//DeclareDataOnPinIfNecessary<float>(pins[2]);
	//memcpy(pins[2].myData, &rotation.z, sizeof(float));

	return -1;
}