#include "stdafx.h"
#include "NodeTypeStartSetCustom.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeStartSetCustom::CNodeTypeStartSetCustom()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
}

int CNodeTypeStartSetCustom::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	bool input = CNodeDataManager::Get()->GetData<bool>(myNodeDataKey);
	CNodeDataManager::Get()->SetData(myNodeDataKey, CNodeDataManager::EDataType::EStart, input);

	return -1;
}