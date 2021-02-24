#include "stdafx.h"
#include "NodeTypeSetGameObjectPosition.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "Engine.h"

CNodeTypeSetGameObjectPosition::CNodeTypeSetGameObjectPosition()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Flow));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT));
    myPins.push_back(SPin("ID", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Int));
    myPins.push_back(SPin("X", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));
    myPins.push_back(SPin("Y", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Float));
}

int CNodeTypeSetGameObjectPosition::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	//TODO
	//Figure out något sätt att accessa myIDGameObjectMap från CGraphManager!


	//aTriggeringNodeInstance->get
	
	//CEngine::GetInstance()->GetActiveScene().FindObjectWithID()

	//SPin::EPinType outType;
	//NodeDataPtr someData = nullptr;
	//size_t outSize = 0;

	//GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	//int id = NodeData::Get<int>(someData);

	//GetDataOnPin(aTriggeringNodeInstance, 3, outType, someData, outSize);
	//float x = NodeData::Get<float>(someData);

	//GetDataOnPin(aTriggeringNodeInstance, 4, outType, someData, outSize);
	//float y = NodeData::Get<float>(someData);

	//CScene::GetActiveScene()->SetPositionForID(id, { x, y });
	return 1;
}
