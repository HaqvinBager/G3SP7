#include "stdafx.h"
#include "NodeTypeListTest.h"
#include "NodeDataManager.h"

CNodeTypeListTest::CNodeTypeListTest()
{
	myPins.push_back(SPin("list", SPin::EPinTypeInOut::EPinTypeInOut_IN));								//0
	myPins.push_back(SPin("list", SPin::EPinTypeInOut::EPinTypeInOut_OUT));								//1
	myPins.push_back(SPin("list", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EList));		//2
	myPins.push_back(SPin("list", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EString));	//3
	std::vector<std::string> list = {"hello", "henlo", "bye"};
	CNodeDataManager::Get()->SetData("List Test", CNodeDataManager::EDataType::EList, list);
}

int CNodeTypeListTest::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	aTriggeringNodeInstance;


    return -1;
}
