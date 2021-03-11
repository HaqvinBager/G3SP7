#include "stdafx.h"
#include "NodeTypeAudioPlayRobotAttack.h"
#include "NodeInstance.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CNodeTypeAudioPlayRobotAttack::CNodeTypeAudioPlayRobotAttack()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
}

int CNodeTypeAudioPlayRobotAttack::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayRobotAttackSound, 0 });
	return -1;
}
