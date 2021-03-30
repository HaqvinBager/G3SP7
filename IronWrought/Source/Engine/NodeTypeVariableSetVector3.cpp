#include "stdafx.h"
#include "NodeTypeVariableSetVector3.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableSetVector3::CNodeTypeVariableSetVector3()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3));
	DeclareDataOnPinIfNecessary<Vector3>(myPins[2], Vector3(1.0f,1.0f,1.0f));
	DeclareDataOnPinIfNecessary<Vector3>(myPins[3], Vector3(1.0f,1.0f,1.0f));
}

int CNodeTypeVariableSetVector3::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	DirectX::SimpleMath::Vector3 input;
	//c[0] = aPin.myData[0];
	memcpy(&input, someData, sizeof(DirectX::SimpleMath::Vector3));

	CNodeDataManager::Get()->SetData(myNodeDataKey, CNodeDataManager::EDataType::EVector3, input);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float*>(pins[3]);
	memcpy(pins[3].myData, &input, sizeof(float*));

	return 1;
}