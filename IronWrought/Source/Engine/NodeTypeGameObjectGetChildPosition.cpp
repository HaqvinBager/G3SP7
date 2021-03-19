#include "stdafx.h"
#include "NodeTypeGameObjectGetChildPosition.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "GameObject.h"
#include "GraphManager.h"
#include "NodeDataManager.h"

CNodeTypeGameObjectGetChildPosition::CNodeTypeGameObjectGetChildPosition()
{
	myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3)); //0
	//myPins.push_back(SPin("Y", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat)); //1
	//myPins.push_back(SPin("Z", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat)); //2
}

int CNodeTypeGameObjectGetChildPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	int index = CNodeDataManager::Get()->GetData<int>(myNodeName);
	std::vector<CGameObject*> gameObject = aTriggeringNodeInstance->GetCurrentGameObject();
	Vector3 position = gameObject[index]->myTransform->Position();

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[0]);
	memcpy(pins[0].myData, &position, sizeof(Vector3));

	return -1;
}