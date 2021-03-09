#include "stdafx.h"
#include "NodeTypeAudioPlayRobotIdle.h"
#include "NodeInstance.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CNodeTypeAudioPlayRobotIdle::CNodeTypeAudioPlayRobotIdle()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
}

int CNodeTypeAudioPlayRobotIdle::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayRobotIdleSound, 0 });
	return -1;
}
