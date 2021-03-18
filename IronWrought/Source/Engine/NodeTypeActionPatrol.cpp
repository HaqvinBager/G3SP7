#include "stdafx.h"
#include "NodeTypeActionPatrol.h"
#include "NodeInstance.h"
//#include "PatrolActionNode.h"
#include "DecisionNode.h"
#include "DecisionTreeManager.h"

CNodeTypeActionPatrol::CNodeTypeActionPatrol()
{
	myPins.push_back(SPin("Value", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EString));	//0
	myPins.push_back(SPin("ID", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EInt));		//1
}

int CNodeTypeActionPatrol::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	//SPin::EPinType outType;
	//NodeDataPtr someData = nullptr;
	//size_t outSize = 0;

	//if (myActions[aTriggeringNodeInstance] == nullptr)
	//{
	//	std::vector<CGameObject*> gameObjects = aTriggeringNodeInstance->GetCurrentGameObject();
	//	std::vector<Vector3> waypoints;

	//	for (int i = 2; i < gameObjects.size(); ++i)
	//	{
	//		waypoints.push_back(gameObjects[i]->myTransform->Position());
	//	}

	//	myActions[aTriggeringNodeInstance] = new CPatrolActionNode(waypoints);
	//}

	//GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	//std::string value = static_cast<char*>(someData);
	//myActions[aTriggeringNodeInstance]->TextToPrint(value);

	//std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();

	//DeclareDataOnPinIfNecessary<int>(pins[1]);
	//int ID = myActions[aTriggeringNodeInstance]->ID();
	//memcpy(pins[1].myData, &ID, sizeof(int));

	return -1;
}

//void CNodeTypeActionPrint::ClearNodeInstanceFromMap(CNodeInstance* aTriggeringNodeInstance)
//{
//	for (auto it : myActions)
//	{
//		if (it.first == aTriggeringNodeInstance)
//		{
//			CDecisionTreeManager::Get()->RemoveNodeWithID(myActions[it.first]->ID());
//		}
//	}
//}
