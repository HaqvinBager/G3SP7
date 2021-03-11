#include "stdafx.h"
#include "NodeTypeAudioPlayRobotPatrolling.h"
#include "NodeInstance.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CNodeTypeAudioPlayRobotPatrolling::CNodeTypeAudioPlayRobotPatrolling()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
}

int CNodeTypeAudioPlayRobotPatrolling::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayRobotPatrolling, 0 });
	return -1;
}
