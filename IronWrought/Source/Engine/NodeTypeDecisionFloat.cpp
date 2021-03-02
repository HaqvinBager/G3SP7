#include "stdafx.h"
#include "NodeTypeDecisionFloat.h"
#include "NodeInstance.h"
#include "FloatDecisionNode.h"
#include "DecisionTreeManager.h"

CNodeTypeDecisionFloat::CNodeTypeDecisionFloat()
{
	myPins.push_back(SPin("True", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EInt));		//0
	myPins.push_back(SPin("ID", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EInt));		//1
	myPins.push_back(SPin("False", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EInt));		//2
	myPins.push_back(SPin("Max", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));		//3
	myPins.push_back(SPin("Min", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));		//4
	myPins.push_back(SPin("Value", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));	//5
}

int CNodeTypeDecisionFloat::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	if (myDecisions[aTriggeringNodeInstance] == nullptr)
	{
		myDecisions[aTriggeringNodeInstance] = new CFloatDecisionNode();
	}

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	int trueID = NodeData::Get<int>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize); 
	int falseID = NodeData::Get<int>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	myDecisions[aTriggeringNodeInstance]->MaxValue(NodeData::Get<float>(someData));

	GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	myDecisions[aTriggeringNodeInstance]->MinValue(NodeData::Get<float>(someData));

	GetDataOnPin(aTriggeringNodeInstance, 5, outType, someData, outSize);
	float value = NodeData::Get<float>(someData);
	myDecisions[aTriggeringNodeInstance]->TestValue(&value);

	if (trueID > 0)
	{
		IBaseDecisionNode* node = CDecisionTreeManager::Get()->GetNodeFromID(trueID);

		if (node != nullptr)
		{
			if (myDecisions[aTriggeringNodeInstance]->TrueNode() != nullptr)
			{
				if (node->ID() != myDecisions[aTriggeringNodeInstance]->TrueNode()->ID())
				{
					myDecisions[aTriggeringNodeInstance]->TrueNode(node);
				}
			}
			else 
			{
				myDecisions[aTriggeringNodeInstance]->TrueNode(node);
			}
		}
	}
	if (falseID > 0)
	{
		IBaseDecisionNode* node = CDecisionTreeManager::Get()->GetNodeFromID(falseID);

		if (node != nullptr)
		{
			if (myDecisions[aTriggeringNodeInstance]->FalseNode() != nullptr)
			{
				if (node->ID() != myDecisions[aTriggeringNodeInstance]->FalseNode()->ID())
				{
					myDecisions[aTriggeringNodeInstance]->FalseNode(node);
				}
			}
			else 
			{
				myDecisions[aTriggeringNodeInstance]->FalseNode(node);
			}
		}
	}

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();

	DeclareDataOnPinIfNecessary<int>(pins[1]);
	int ID = myDecisions[aTriggeringNodeInstance]->ID();
	memcpy(pins[1].myData, &ID, sizeof(int));

	return -1;
}

void CNodeTypeDecisionFloat::ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance)
{
	for (auto it : myDecisions)
	{
		if (it.first == aTriggeringNodeInstance)
		{
			CDecisionTreeManager::Get()->RemoveNodeWithID(myDecisions[it.first]->ID());
		}
	}
}
