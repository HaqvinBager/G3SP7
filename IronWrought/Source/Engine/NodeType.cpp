#include "stdafx.h"
#include "NodeType.h"
#include "NodeTypeStartDefault.h"
#include "NodeTypeStartKeyboardInput.h"
#include "NodeTypeDebugPrint.h"
#include "NodeInstance.h"
#include "NodeTypeGameObjectGetPosition.h"
#include "NodeTypeGameObjectSetPosition.h"
#include "NodeTypeGameObjectMove.h"
#include "NodeTypeGameObjectMoveToPosition.h"
#include "NodeTypeGameObjectGetRotation.h"
#include "NodeTypeGameObjectSetRotation.h"
#include "NodeTypeGameObjectRotate.h"
#include "NodeTypeMathAdd.h"
#include "NodeTypeMathSub.h"
#include "NodeTypeMathMul.h"
#include "NodeTypeMathCos.h"
#include "NodeTypeMathSin.h"
#include "NodeTypeMathAbs.h"
#include "NodeTypeMathAtan2.h"
#include "NodeTypeMathCeil.h"
#include "NodeTypeMathFloor.h"
#include "NodeTypeMathGreater.h"
#include "NodeTypeMathToRadians.h"
#include "NodeTypeMathToDegrees.h"
#include "NodeTypeMathDot.h"
#include "NodeTypeMathLerp.h"
#include "NodeTypeMathSaturate.h"
#include "NodeTypeMathSmoothstep.h"
#include "NodeTypeMathMax.h"
#include "NodeTypeMathMin.h"
#include "NodeTypeMathDistance.h"
#include "NodeTypeInputGetMousePosition.h"
#include "NodeTypeTimeTimer.h"
#include "NodeTypeTimeDeltaTotal.h"
#include "NodeTypeActionPrint.h"
#include "NodeTypeDecisionFloat.h"
#include "NodeTypeDecisionTreeRun.h"
#include "NodeTypeVariableSetFloat.h"
#include "NodeTypeVariableSetVector3.h"
#include "NodeTypeVariableSetInt.h"
#include "NodeTypeVariableSetBool.h"
#include "NodeTypeVariableGetFloat.h"
#include "NodeTypeVariableGetVector3.h"
#include "NodeTypeVariableGetInt.h"
#include "NodeTypeVariableGetBool.h"
#include "NodeTypeStartGetCustom.h"
#include "NodeTypeStartSetCustom.h"
#include "NodeTypeDecisionCoinFlip.h"
#include "NodeTypeUtilitiesBranching.h"
#include "NodeTypeAudioPlayStep.h"
#include "NodeTypeAudioPlayResearcherReactionExplosives.h"
#include "NodeTypeAudioPlayRobotAttack.h"
#include "NodeTypeAudioPlayRobotDeath.h"
#include "NodeTypeAudioPlayRobotIdle.h"
#include "NodeTypeAudioPlayRobotPatrolling.h"
#include "NodeTypeAudioPlayRobotSearching.h"
#include "NodeTypeListTest.h"
#include "NodeTypeVFXPlayVFX.h"
#include "NodeTypeVFXStopVFX.h"
#include "NodeTypeAudioPlayResearcherEvent.h"
#include "NodeTypeAudioPlaySFX.h"

CNodeType* CNodeTypeCollector::myTypes[128];
unsigned short CNodeTypeCollector::myTypeCounter = 0;
unsigned short CNodeTypeCollector::myTypeCount = 0;

std::vector<unsigned int> CUID::myAllUIDs;
unsigned int CUID::myGlobalUID = 0;

void CNodeTypeCollector::PopulateTypes()
{
	RegisterType<CNodeTypeStartDefault>("Default Start");
	RegisterType<CNodeTypeStartKeyboardInput>("Keyboard Input Start");
	RegisterType<CNodeTypeDebugPrint>("Print Message");
	RegisterType<CNodeTypeMathAdd>("Add");
	RegisterType<CNodeTypeMathSub>("Sub");
	RegisterType<CNodeTypeMathMul>("Mul");
	RegisterType<CNodeTypeMathCos>("Cos");
	RegisterType<CNodeTypeMathSin>("Sin");
	RegisterType<CNodeTypeMathAbs>("Abs");
	RegisterType<CNodeTypeMathAtan2>("Atan2");
	RegisterType<CNodeTypeMathCeil>("Ceil");
	RegisterType<CNodeTypeMathFloor>("Floor");
	RegisterType<CNodeTypeMathToRadians>("To Radians");
	RegisterType<CNodeTypeMathToDegrees>("To Degrees");
	RegisterType<CNodeTypeMathDot>("Dot");
	RegisterType<CNodeTypeMathLerp>("Lerp");
	RegisterType<CNodeTypeMathSaturate>("Saturate");
	RegisterType<CNodeTypeMathSmoothstep>("Smoothstep");
	RegisterType<CNodeTypeMathMax>("Max");
	RegisterType<CNodeTypeMathMin>("Min");
	RegisterType<CNodeTypeMathGreater>("Greater");
	RegisterType<CNodeTypeMathDistance>("Distance");
	RegisterType<CNodeTypeGameObjectGetPosition>("Get Object Position");
	RegisterType<CNodeTypeGameObjectSetPosition>("Set Object Position");
	RegisterType<CNodeTypeGameObjectMove>("Move Object");
	RegisterType<CNodeTypeGameObjectMoveToPosition>("Move Object To Position");
	RegisterType<CNodeTypeGameObjectGetRotation>("Get Object Rotation");
	RegisterType<CNodeTypeGameObjectSetRotation>("Set Object Rotation");
	RegisterType<CNodeTypeGameObjectRotate>("Rotate Object");
	RegisterType<CNodeTypeInputGetMousePosition>("Mouse Position");
	RegisterType<CNodeTypeActionPrint>("Print Action");
	RegisterType<CNodeTypeDecisionFloat>("Float Decision");
	RegisterType<CNodeTypeDecisionCoinFlip>("Coin Flip Decision");
	RegisterType<CNodeTypeDecisionTreeRun>("Run Tree");
	RegisterType<CNodeTypeTimeDeltaTotal>("Delta & Total Time");
	RegisterType<CNodeTypeTimeTimer>("Countdown Timer");
	RegisterType<CNodeTypeUtilitiesBranching>("Branching");
	RegisterType<CNodeTypeAudioPlayStep>("Play Step");
	RegisterType<CNodeTypeAudioPlayResearcherReactionExplosives>("Play Researcher Explosives Reaction");
	RegisterType<CNodeTypeAudioPlayRobotAttack>("Play Robot Attack");
	RegisterType<CNodeTypeAudioPlayRobotDeath>("Play Robot Death");
	RegisterType<CNodeTypeAudioPlayRobotIdle>("Play Robot Idle");
	RegisterType<CNodeTypeAudioPlayRobotPatrolling>("Play Robot Patrolling");
	RegisterType<CNodeTypeAudioPlayRobotSearching>("Play Robot Searching");
	RegisterType<CNodeTypeListTest>("List Test");
	RegisterType<CNodeTypeVFXPlayVFX>("Play VFX");
	RegisterType<CNodeTypeVFXStopVFX>("Stop VFX");
	RegisterType<CNodeTypeAudioPlayResearcherEvent>("Play Researcher Event");
	RegisterType<CNodeTypeAudioPlaySFX>("Play SFX");
}

void CNodeTypeCollector::RegisterNewDataType(std::string aNodeName, unsigned int aType)
{
	switch (aType)
	{
	case 0:
	{
		RegisterDataType<CNodeTypeVariableSetFloat>("Set: " + aNodeName, aNodeName);
		RegisterDataType<CNodeTypeVariableGetFloat>("Get: " + aNodeName, aNodeName);
	}
		break;
	case 1:
	{
		RegisterDataType<CNodeTypeVariableSetInt>("Set: " + aNodeName, aNodeName);
		RegisterDataType<CNodeTypeVariableGetInt>("Get: " + aNodeName, aNodeName);
	}
		break;
	case 2:
	{
		RegisterDataType<CNodeTypeVariableSetBool>("Set: " + aNodeName, aNodeName);
		RegisterDataType<CNodeTypeVariableGetBool>("Get: " + aNodeName, aNodeName);
	}
		break;
	case 3:
	{
		RegisterDataType<CNodeTypeStartSetCustom>("Set: " + aNodeName, aNodeName);
		RegisterDataType<CNodeTypeStartGetCustom>("Get: " + aNodeName, aNodeName);
	}
		break;
	case 4:
	{
		RegisterDataType<CNodeTypeVariableSetVector3>("Set: " + aNodeName, aNodeName);
		RegisterDataType<CNodeTypeVariableGetVector3>("Get: " + aNodeName, aNodeName);
	}
		break;
	default:
		break;
	}
}

void CNodeType::ClearNodeInstanceFromMap(CNodeInstance* /*aTriggeringNodeInstance*/)
{
}

int CNodeType::DoEnter(CNodeInstance* aTriggeringNodeInstance)
{
	int index = OnEnter(aTriggeringNodeInstance);
	aTriggeringNodeInstance->myEnteredTimer = 3.0f;
	return index;
}

std::vector<SPin> CNodeType::GetPins()
{
	return myPins;
}

void CNodeType::GetDataOnPin(CNodeInstance* aTriggeringNodeInstance, unsigned int aPinIndex, SPin::EPinType& outType, void*& someData, size_t& outSize)
{
	aTriggeringNodeInstance->FetchData(outType, someData, outSize, aPinIndex);
}
//
//void CNodeType::GetDataOnPin(CNodeInstance* aTriggeringNodeInstance, unsigned int aPinIndex, SPin::EPinType& outType, NodeDataPtr& someData, size_t& outSize)
//{
//	
//}
