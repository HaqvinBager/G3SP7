#include "stdafx.h"
#include "NodeTypeVariableGetFloat.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableGetFloat::CNodeTypeVariableGetFloat()
{
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));		//0
	myPins.push_back(SPin("Local", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));		//1
}

int CNodeTypeVariableGetFloat::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject();

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	bool local = NodeData::Get<bool>(someData);
	float output;

	if (!local)
		output = CNodeDataManager::Get()->GetData<float>(myNodeDataKey);
	else
		output = CNodeDataManager::Get()->GetData<float>(myNodeDataKey + std::to_string(gameObject->InstanceID()));

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[0]);
	memcpy(pins[0].myData, &output, sizeof(float));

	return -1;
}