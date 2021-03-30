#include "stdafx.h"
#include "NodeTypeVariableGetVector3.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableGetVector3::CNodeTypeVariableGetVector3()
{
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3));
	DeclareDataOnPinIfNecessary<Vector3>(myPins[0], Vector3(1.0f,1.0f,1.0f));
}

int CNodeTypeVariableGetVector3::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	float* output = CNodeDataManager::Get()->GetData<float*>(myNodeDataKey);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float*>(pins[0]);
	memcpy(pins[0].myData, &output, sizeof(float*));

	return -1;
}