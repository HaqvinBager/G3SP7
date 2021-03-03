#include "stdafx.h"
#include "NodeTypeVariableGetInt.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableGetInt::CNodeTypeVariableGetInt()
{
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EInt));
}

int CNodeTypeVariableGetInt::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	int output = *(static_cast<int*>(CNodeDataManager::Get()->GetData(myNodeDataKey, CNodeDataManager::EDataType::EInt)));

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<int>(pins[0]);
	memcpy(pins[0].myData, &output, sizeof(int));

	return -1;
}