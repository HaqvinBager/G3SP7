#include "stdafx.h"
#include "NodeTypeVariableGetBool.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"
#include "Timer.h"
#include "Scene.h"
#include "Engine.h"
#include "GraphManager.h"

CNodeTypeVariableGetBool::CNodeTypeVariableGetBool()
{
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EBool));		//0
	myPins.push_back(SPin("Local", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));		//1
}

int CNodeTypeVariableGetBool::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(aTriggeringNodeInstance->GraphManager()->GetCurrentBlueprintInstanceID());

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;
	
	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	
	bool input = NodeData::Get<bool>(someData);
	bool output;

	if(!input)
		output = CNodeDataManager::Get()->GetData<bool>(myNodeDataKey);
	else
		output = CNodeDataManager::Get()->GetData<bool>(std::to_string(gameObject->InstanceID()));

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<bool>(pins[0]);
	memcpy(pins[0].myData, &output, sizeof(bool));

	return -1;
}