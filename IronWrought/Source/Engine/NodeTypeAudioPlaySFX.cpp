#include "stdafx.h"
#include "NodeTypeAudioPlaySFX.h"
#include "NodeDataManager.h"
#include "NodeInstance.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CNodeTypeAudioPlaySFX::CNodeTypeAudioPlaySFX()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
	myPins.push_back(SPin("Clip", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EInt));
	//std::vector<std::string> list = { "GravityGlovePullBuildup\0", "GravityGlovePullHit\0", "GravityGlovePush\0" };
	//CNodeDataManager::Get()->SetData("Play SFX", CNodeDataManager::EDataType::EStringList, list);
}

int CNodeTypeAudioPlaySFX::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize); // Get data on pin index 1, this index is relative to what you push in the constructor
	int input = NodeData::Get<int>(someData);


	if (input < 0)
	{
		input = 0;
	}
	else if (input > myMaxValue)
	{
		input = myMaxValue;
	}

	CMainSingleton::PostMaster().Send({ EMessageType::PlaySFX, &input });

	return -1;
}
