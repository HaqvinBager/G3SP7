#include "stdafx.h"
#include "NodeTypeDecisionTreeRun.h"
#include "NodeInstance.h"
#include "BaseDecisionNode.h"
#include "DecisionTreeManager.h"

CNodeTypeDecisionTreeRun::CNodeTypeDecisionTreeRun()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::PinTypeInOut_IN));							//0
	myPins.push_back(SPin("", SPin::EPinTypeInOut::PinTypeInOut_OUT));							//1
	myPins.push_back(SPin("ID", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Int));	//2
}

int CNodeTypeDecisionTreeRun::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	int ID = NodeData::Get<int>(someData);

	if (myTreesToRun[aTriggeringNodeInstance] == nullptr)
	{
		myTreesToRun[aTriggeringNodeInstance] = CDecisionTreeManager::Get()->GetNodeFromID(ID);
	}

	if (myTreesToRun[aTriggeringNodeInstance] != nullptr)
	{
		if (myTreesToRun[aTriggeringNodeInstance]->ID() != ID)
		{
			myTreesToRun[aTriggeringNodeInstance] = CDecisionTreeManager::Get()->GetNodeFromID(ID);
		}
		CDecisionTreeManager::Get()->RunTree(myTreesToRun[aTriggeringNodeInstance]);
	}

	return 1;
}

void CNodeTypeDecisionTreeRun::ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance)
{
	for (auto& it : myTreesToRun)
	{
		if (it.first == aTriggeringNodeInstance)
		{
			if (myTreesToRun[it.first] != nullptr)
			{
				CDecisionTreeManager::Get()->RemoveNodeWithID(myTreesToRun[it.first]->ID());
			}
		}
	}
}