#include "stdafx.h"
#include "NodeTypeActionPrint.h"
#include "NodeInstance.h"
#include "PrintActionNode.h"
#include "DecisionNode.h"
#include "DecisionTreeManager.h"

CNodeTypeActionPrint::CNodeTypeActionPrint()
{
	myPins.push_back(SPin("Value", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EString));	//0
	myPins.push_back(SPin("ID", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EInt));		//1
}

int CNodeTypeActionPrint::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	if (myActions[aTriggeringNodeInstance] == nullptr)
	{
		myActions[aTriggeringNodeInstance] = new CPrintActionNode();
	}

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	std::string value = static_cast<char*>(someData);
	myActions[aTriggeringNodeInstance]->TextToPrint(value);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();

	DeclareDataOnPinIfNecessary<int>(pins[1]);
	int ID = myActions[aTriggeringNodeInstance]->ID();
	memcpy(pins[1].myData, &ID, sizeof(int));

	return -1;
}

void CNodeTypeActionPrint::ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance)
{
	for (auto it : myActions)
	{
		if (it.first == aTriggeringNodeInstance)
		{
			CDecisionTreeManager::Get()->RemoveNodeWithID(myActions[it.first]->ID());
		}
	}
}
