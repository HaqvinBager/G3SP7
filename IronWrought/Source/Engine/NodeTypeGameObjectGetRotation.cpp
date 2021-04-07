#include "stdafx.h"
#include "NodeTypeGameObjectGetRotation.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "Engine.h"
#include "GameObject.h"
#include "GraphManager.h"

CNodeTypeGameObjectGetRotation::CNodeTypeGameObjectGetRotation()
{
	myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3));
}

int CNodeTypeGameObjectGetRotation::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(aTriggeringNodeInstance->GraphManager()->GetCurrentBlueprintInstanceID());
	Vector3 rotation = gameObject->myTransform->Position();

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[0]);
	memcpy(pins[0].myData, &rotation, sizeof(Vector3));

	return -1;
}