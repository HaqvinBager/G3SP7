#include "stdafx.h"
#include "NodeTypeAudioPlayResearcherReactionExplosives.h"
#include "NodeInstance.h"
#include "MainSingleton.h"
#include "PostMaster.h"

CNodeTypeAudioPlayResearcherReactionExplosives::CNodeTypeAudioPlayResearcherReactionExplosives()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
}

int CNodeTypeAudioPlayResearcherReactionExplosives::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	CMainSingleton::PostMaster().SendLate({ EMessageType::PlayResearcherReactionExplosives, 0 });
	return -1;
}
