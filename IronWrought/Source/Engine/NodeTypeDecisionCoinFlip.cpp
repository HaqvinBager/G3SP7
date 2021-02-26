#include "stdafx.h"
#include "NodeTypeDecisionCoinFlip.h"
#include "NodeInstance.h"
#include "FlipACoinDecision.h"
#include "DecisionTreeManager.h"

CNodeTypeDecisionCoinFlip::CNodeTypeDecisionCoinFlip()
{
	myPins.push_back(SPin("True", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Int));		//0
	myPins.push_back(SPin("ID", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Int));		//1
	myPins.push_back(SPin("False", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Int));		//2
	myPins.push_back(SPin("Time", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));	//3

}

int CNodeTypeDecisionCoinFlip::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	if (myDecisions[aTriggeringNodeInstance] == nullptr)
	{
		myDecisions[aTriggeringNodeInstance] = new CFlipACoinDecision();
	}

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	int trueID = NodeData::Get<int>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	int falseID = NodeData::Get<int>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	myDecisions[aTriggeringNodeInstance]->SetMaxTime(NodeData::Get<float>(someData));
	

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

void CNodeTypeDecisionCoinFlip::ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance)
{
	for (auto it : myDecisions)
	{
		if (it.first == aTriggeringNodeInstance)
		{
			CDecisionTreeManager::Get()->RemoveNodeWithID(myDecisions[it.first]->ID());
		}
	}
}
