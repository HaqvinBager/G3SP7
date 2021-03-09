#include "stdafx.h"
#include "NodeTypeAudioPlayStep.h"
#include "NodeInstance.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CNodeTypeAudioPlayStep::CNodeTypeAudioPlayStep()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
}

int CNodeTypeAudioPlayStep::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayStepSound, 0 });
	return -1;
}
