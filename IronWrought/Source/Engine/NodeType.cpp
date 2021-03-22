#include "stdafx.h"
#include "NodeType.h"
#include "NodeTypeStartDefault.h"
#include "NodeTypeStartKeyboardInput.h"
#include "NodeTypeDebugPrint.h"
#include "NodeInstance.h"
#include "NodeTypeGameObjectGetPosition.h"
#include "NodeTypeGameObjectGetChildPosition.h"
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
#include "NodeTypeVector3Dot.h"
#include "NodeTypeMathLerp.h"
#include "NodeTypeMathSaturate.h"
#include "NodeTypeMathSmoothstep.h"
#include "NodeTypeMathMax.h"
#include "NodeTypeMathMin.h"
#include "NodeTypeVector3Distance.h"
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
#include "NodeTypeVector3Add.h"
#include "NodeTypeVector3Sub.h"
#include "NodeTypeVector3Split.h"
#include "NodeTypeVector3Join.h"
#include "NodeTypeVector3Cross.h"
#include "NodeTypeStartSphereOnTriggerEnter.h"
#include "Scene.h"
#include "Engine.h"
#include "NodeDataManager.h"

CNodeType* CNodeTypeCollector::myTypes[128];
unsigned short CNodeTypeCollector::myTypeCounter = 0;
std::unordered_map<std::string, SNodeTypeData> CNodeTypeCollector::myChildNodeTypesMap;

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
	RegisterType<CNodeTypeVector3Dot>("Vec3 Dot");
	RegisterType<CNodeTypeMathLerp>("Lerp");
	RegisterType<CNodeTypeMathSaturate>("Saturate");
	RegisterType<CNodeTypeMathSmoothstep>("Smoothstep");
	RegisterType<CNodeTypeMathMax>("Max");
	RegisterType<CNodeTypeMathMin>("Min");
	RegisterType<CNodeTypeMathGreater>("Greater");
	RegisterType<CNodeTypeVector3Distance>("Vec3 Distance");
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
	RegisterType<CNodeTypeVector3Add>("Vec3 Add");
	RegisterType<CNodeTypeVector3Sub>("Vec3 Sub");
	RegisterType<CNodeTypeVector3Split>("Vec3 Split");
	RegisterType<CNodeTypeVector3Join>("Vec3 Join");
	RegisterType<CNodeTypeVector3Cross>("Vec3 Cross");
	RegisterType<CNodeTypeStartSphereOnTriggerEnter>("Sphere On Trigger Enter");

}

void CNodeTypeCollector::RegisterNewDataType(const std::string& aNodeName, unsigned int aType)
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

void CNodeTypeCollector::RegisterChildNodeTypes(std::string aKey, const unsigned int aNumberOfChildren)
{
	std::string name;
	int index;
	for (unsigned int i = 0; i < aNumberOfChildren; ++i)
	{
		index = i + 2;
		name = "Get " + aKey + " Child " + std::to_string(index - 2) + " Position";
		RegisterChildType<CNodeTypeGameObjectGetChildPosition>(aKey, name);
		CNodeDataManager::Get()->SetData(name, CNodeDataManager::EDataType::EInt, index);
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

void CNodeType::GetDataOnPin(CNodeInstance* aTriggeringNodeInstance, unsigned int aPinIndex, SPin::EPinType& anOutType, void*& someData, size_t& anOutSize)
{
	aTriggeringNodeInstance->FetchData(anOutType, someData, anOutSize, aPinIndex);
}