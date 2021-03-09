#include "stdafx.h"
#include "NodeTypeAudioPlayRobotDeath.h"
#include "NodeInstance.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CNodeTypeAudioPlayRobotDeath::CNodeTypeAudioPlayRobotDeath()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
}

int CNodeTypeAudioPlayRobotDeath::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayRobotDeathSound, 0 });
	return -1;
}
