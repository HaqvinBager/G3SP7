#include "stdafx.h"
#include "NodeTypeListTest.h"
#include "NodeDataManager.h"
#include "NodeInstance.h"

CNodeTypeListTest::CNodeTypeListTest()
{
	myPins.push_back(SPin("list", SPin::EPinTypeInOut::EPinTypeInOut_IN));								//0
	myPins.push_back(SPin("list", SPin::EPinTypeInOut::EPinTypeInOut_OUT));								//1
	myPins.push_back(SPin("list", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EStringList));	//2
	myPins.push_back(SPin("list", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EString));	//3
	std::vector<std::string> list = {"hello\0", "henlo\0", "bye\0"};
	CNodeDataManager::Get()->SetData("List Test", CNodeDataManager::EDataType::EStringList, list);
}

int CNodeTypeListTest::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	std::string input = static_cast<char*>(someData);
	std::cout << input << std::endl;
	//std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	//DeclareDataOnPinIfNecessary<char*>(pins[3]);
	//memcpy(pins[3].myData, input.data(), input.length());

    return 1;
}
