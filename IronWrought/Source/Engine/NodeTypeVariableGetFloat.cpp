#include "stdafx.h"
#include "NodeTypeVariableGetFloat.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableGetFloat::CNodeTypeVariableGetFloat()
{
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));
}

int CNodeTypeVariableGetFloat::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	float output = CNodeDataManager::Get()->GetData<float>(myNodeDataKey);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[0]);
	memcpy(pins[0].myData, &output, sizeof(float));

	return -1;
}