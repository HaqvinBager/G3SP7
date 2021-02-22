#include "stdafx.h"
#include "NodeTypeForEachGameObject.h"
#include "NodeInstance.h"
//#include "Scene.h"
//#include "GameObject.h"

CNodeTypeForEachGameObject::CNodeTypeForEachGameObject()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::PinTypeInOut_IN));
	myPins.push_back(SPin("Type", SPin::EPinTypeInOut::PinTypeInOut_IN, SPin::EPinType::String));
	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::PinTypeInOut_OUT));
	myPins.push_back(SPin("OnIter", SPin::EPinTypeInOut::PinTypeInOut_OUT));
	myPins.push_back(SPin("Index", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Int));
	myPins.push_back(SPin("ID", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Int));
}

int CNodeTypeForEachGameObject::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;
	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	std::string input = static_cast<char*>(someData);
	//std::vector<CGameObject*> gameObjects = CScene::GetActiveScene()->GetObjectsOfType(input);

	//std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	//int ID;

	//for (unsigned int i = 0; i < gameObjects.size(); ++i) {
	//	DeclareDataOnPinIfNecessary<int>(pins[4]);
	//	memcpy(pins[4].myData, &i, sizeof(int));

	//	DeclareDataOnPinIfNecessary<int>(pins[5]);
	//	ID = gameObjects[i]->GetID();
	//	memcpy(pins[5].myData, &ID, sizeof(int));

	//	aTriggeringNodeInstance->ExitVia(3);
	//}

	return 2;
}
