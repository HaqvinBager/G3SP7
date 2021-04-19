#include "stdafx.h"
#include "NodeTypeVariableGetInt.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableGetInt::CNodeTypeVariableGetInt()
{
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EInt));		//0
	myPins.push_back(SPin("Local", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));		//1
}

int CNodeTypeVariableGetInt::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject();

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	bool local = NodeData::Get<bool>(someData);

	int output;

	if (!local)
		output = CNodeDataManager::Get()->GetData<int>(myNodeDataKey);
	else
		output = CNodeDataManager::Get()->GetData<int>(myNodeDataKey + std::to_string(gameObject->InstanceID()));

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<int>(pins[0]);
	memcpy(pins[0].myData, &output, sizeof(int));

	return -1;
}