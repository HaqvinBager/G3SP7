#include "stdafx.h"
#include "NodeTypeAudioPlayRobotSearching.h"
#include "NodeInstance.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CNodeTypeAudioPlayRobotSearching::CNodeTypeAudioPlayRobotSearching()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
}

int CNodeTypeAudioPlayRobotSearching::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayRobotSearching, 0 });
	return -1;
}
