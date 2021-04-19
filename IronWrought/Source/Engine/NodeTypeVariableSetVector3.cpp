#include "stdafx.h"
#include "NodeTypeVariableSetVector3.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableSetVector3::CNodeTypeVariableSetVector3()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));									//0
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));									//1
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));		//2
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3));		//3
	myPins.push_back(SPin("Local", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EBool));		//4
}

int CNodeTypeVariableSetVector3::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject();

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	bool local = NodeData::Get<bool>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	DirectX::SimpleMath::Vector3 input;
	memcpy(&input, someData, sizeof(DirectX::SimpleMath::Vector3));

	if (!local)
		CNodeDataManager::Get()->SetData(myNodeDataKey, CNodeDataManager::EDataType::EVector3, input);
	else
		CNodeDataManager::Get()->SetData(myNodeDataKey + std::to_string(gameObject->InstanceID()), CNodeDataManager::EDataType::EVector3, input, false);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float*>(pins[3]);
	memcpy(pins[3].myData, &input, sizeof(float*));

	return 1;
}