#include "stdafx.h"
#include "NodeTypeStartGetCustom.h"
#include <iostream>
#include "NodeType.h"
#include "NodeInstance.h"
#include "NodeDataManager.h"

CNodeTypeStartGetCustom::CNodeTypeStartGetCustom()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));
}

int CNodeTypeStartGetCustom::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	aTriggeringNodeInstance->myShouldTriggerAgain = CNodeDataManager::Get()->GetData<bool>(myNodeDataKey);
	
	if (aTriggeringNodeInstance->myShouldTriggerAgain)
	{
		//aTriggeringNodeInstance->myShouldTriggerAgain = false;
		CNodeDataManager::Get()->SetData(myNodeDataKey, CNodeDataManager::EDataType::EStart, aTriggeringNodeInstance->myShouldTriggerAgain);
		return 0;
	}
	else
		return -1;
}
