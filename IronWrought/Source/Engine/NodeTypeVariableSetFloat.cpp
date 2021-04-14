#include "stdafx.h"
#include "NodeTypeVariableSetFloat.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"
#include "Timer.h"
#include "Scene.h"
#include "Engine.h"
#include "GraphManager.h"

CNodeTypeVariableSetFloat::CNodeTypeVariableSetFloat()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));									//0
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));									//1
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));		//2
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));			//3
	myPins.push_back(SPin("Local", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));		//4
}

int CNodeTypeVariableSetFloat::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(aTriggeringNodeInstance->GraphManager()->GetCurrentBlueprintInstanceID());

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	bool local = NodeData::Get<bool>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	float input = NodeData::Get<float>(someData);

	if (!local)
		CNodeDataManager::Get()->SetData(myNodeDataKey, CNodeDataManager::EDataType::EFloat, input);
	else
		CNodeDataManager::Get()->SetData(std::to_string(gameObject->InstanceID()), CNodeDataManager::EDataType::EFloat, input, false);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[3]);
	memcpy(pins[3].myData, &input, sizeof(float));

	return 1;
}