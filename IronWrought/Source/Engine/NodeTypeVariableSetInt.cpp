#include "stdafx.h"
#include "NodeTypeVariableSetInt.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeVariableSetInt::CNodeTypeVariableSetInt()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));
	myPins.push_back(SPin("Val", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EInt));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EInt));
	DeclareDataOnPinIfNecessary<int>(myPins[2], 0);
	DeclareDataOnPinIfNecessary<int>(myPins[3], 0);
}

int CNodeTypeVariableSetInt::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	int input = NodeData::Get<int>(someData);
	CNodeDataManager::Get()->SetData(myNodeDataKey, CNodeDataManager::EDataType::EInt, input);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<int>(pins[3]);
	memcpy(pins[3].myData, &input, sizeof(int));

	return 1;
}
