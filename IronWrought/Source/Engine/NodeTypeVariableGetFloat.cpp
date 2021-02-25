#include "stdafx.h"
#include "NodeTypeVariableGetFloat.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableGetFloat::CNodeTypeVariableGetFloat()
{
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float));
}

int CNodeTypeVariableGetFloat::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	float output = *(static_cast<float*>(CNodeDataManager::Get()->GetData(myNodeDataKey, CNodeDataManager::EDataType::EFloat)));

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[0]);
	memcpy(pins[0].myData, &output, sizeof(float));

	return -1;
}