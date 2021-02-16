#include "stdafx.h"
#include "NodeTypeGetGameObjectPosition.h"
#include "NodeInstance.h"
#include "Scene.h"

CNodeTypeGetGameObjectPosition::CNodeTypeGetGameObjectPosition()
{
    myPins.push_back(SPin("ID", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::Int));
    myPins.push_back(SPin("X", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float));
    myPins.push_back(SPin("Y", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float));
}

int CNodeTypeGetGameObjectPosition::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	//SPin::EPinType outType;
	//NodeDataPtr someData = nullptr;
	//size_t outSize = 0;

	//GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	//int input1 = NodeData::Get<int>(someData);

	//Tga2D::Vector2f position = CScene::GetActiveScene()->GetPositionFromID(input1);

	//std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	//DeclareDataOnPinIfNecessary<float>(pins[1]);
	//memcpy(pins[1].myData, &position.x, sizeof(float));
	//DeclareDataOnPinIfNecessary<float>(pins[2]);
	//memcpy(pins[2].myData, &position.y, sizeof(float));

	return -1;
}
