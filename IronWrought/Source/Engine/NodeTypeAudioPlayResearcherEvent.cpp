#include "stdafx.h"
#include "NodeTypeAudioPlayResearcherEvent.h"
#include "NodeDataManager.h"
#include "NodeInstance.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CNodeTypeAudioPlayResearcherEvent::CNodeTypeAudioPlayResearcherEvent()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
	myPins.push_back(SPin("Clip", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EStringListIndexed));
	std::vector<std::string> list = { "DoorEventVerticalSlice\0", "IntroVerticalSlice\0" };
	CNodeDataManager::Get()->SetData("Play Researcher Event", CNodeDataManager::EDataType::EStringList, list);
}

int CNodeTypeAudioPlayResearcherEvent::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize); // Get data on pin index 1, this index is relative to what you push in the constructor
	int input = NodeData::Get<int>(someData);


	CMainSingleton::PostMaster().Send({ EMessageType::PlayResearcherEvent, &input });

	return -1;
}
