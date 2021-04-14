#include "stdafx.h"
#include "NodeTypeVariableGetVector3.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"
#include "Scene.h"
#include "Engine.h"
#include "GraphManager.h"

CNodeTypeVariableGetVector3::CNodeTypeVariableGetVector3()
{
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3));	//0
	myPins.push_back(SPin("Local", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));		//1
}

int CNodeTypeVariableGetVector3::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(aTriggeringNodeInstance->GraphManager()->GetCurrentBlueprintInstanceID());

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	bool local = NodeData::Get<bool>(someData);
	float* output;

	if (!local)
		output = CNodeDataManager::Get()->GetData<float*>(myNodeDataKey);
	else
		output = CNodeDataManager::Get()->GetData<float*>(myNodeDataKey + std::to_string(gameObject->InstanceID()));

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float*>(pins[0]);
	memcpy(pins[0].myData, &output, sizeof(float*));

	return -1;
}