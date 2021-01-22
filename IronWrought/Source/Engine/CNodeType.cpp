#include "stdafx.h"
#include "CNodeType.h"
#include "CNodeTypePrint.h"
#include "CNodeTypeStart.h"
#include "CNodeInstance.h"
#include "CNodeTypeMathAdd.h"
#include "CNodeTypeMathSub.h"
#include "CNodeTypeMathMul.h"
//#include "CNodeTypeGetGameObjectPosition.h"
//#include "CNodeTypeSetGameObjectPosition.h"
#include "CNodeTypeMathCos.h"
#include "CNodeTypeMathSin.h"
#include "CNodeTypeMathAbs.h"
#include "CNodeTypeMathAtan2.h"
//#include "CNodeTypeCreateGameObject.h"
#include "CNodeTypeMathCeil.h"
#include "CNodeTypeMathFloor.h"
#include "CNodeTypeMathToRadians.h"
#include "CNodeTypeMathToDegrees.h"
#include "CNodeTypeMathDot.h"
#include "CNodeTypeMathLerp.h"
#include "CNodeTypeMathSaturate.h"
#include "CNodeTypeMathStorage.h"
#include "CNodeTypeMathSmoothstep.h"
#include "CNodeTypeMathMax.h"
#include "CNodeTypeMathMin.h"
#include "CNodeTypeMathDistance.h"
//#include "CNodeTypeSetGameObjectScale.h"
#include "CNodeTypeGetMousePosition.h"
//#include "CNodeTypeForEachGameObject.h"
//#include "CNodeTypeVolumeTrigger.h"
//#include "CNodeTypeTimer.h"

CNodeType* CNodeTypeCollector::myTypes[128];
unsigned short CNodeTypeCollector::myTypeCounter = 0;
unsigned short CNodeTypeCollector::myTypeCount = 0;

std::vector<unsigned int> UID::myAllUIDs;
unsigned int UID::myGlobalUID = 0;

void CNodeTypeCollector::PopulateTypes()
{
	RegisterType<CNodeTypePrint>();
	RegisterType<CNodeTypeStart>();
	RegisterType<CNodeTypeMathAdd>();
	RegisterType<CNodeTypeMathSub>();
	RegisterType<CNodeTypeMathMul>();
	//RegisterType<CNodeTypeGetGameObjectPosition>();
	//RegisterType<CNodeTypeSetGameObjectPosition>();
	RegisterType<CNodeTypeMathCos>();
	RegisterType<CNodeTypeMathSin>();
	RegisterType<CNodeTypeMathAbs>();
	RegisterType<CNodeTypeMathAtan2>();
	//RegisterType<CNodeTypeCreateGameObject>();
	RegisterType<CNodeTypeMathCeil>();
	RegisterType<CNodeTypeMathFloor>();
	RegisterType<CNodeTypeMathToRadians>();
	RegisterType<CNodeTypeMathToDegrees>();
	RegisterType<CNodeTypeMathDot>();
	RegisterType<CNodeTypeMathLerp>();
	RegisterType<CNodeTypeMathSaturate>();
	RegisterType<CNodeTypeMathStorage>();
	RegisterType<CNodeTypeMathSmoothstep>();
	RegisterType<CNodeTypeMathMax>();
	RegisterType<CNodeTypeMathMin>();
	RegisterType<CNodeTypeMathDistance>();
	//RegisterType<CNodeTypeSetGameObjectScale>();
	RegisterType<CNodeTypeGetMousePosition>();
	//RegisterType<CNodeTypeForEachGameObject>();
	//RegisterType<CNodeTypeVolumeTrigger>();
	//RegisterType<CNodeTypeTimer>();
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

void CNodeType::GetDataOnPin(CNodeInstance* aTriggeringNodeInstance, unsigned int aPinIndex, SPin::PinType& outType, NodeDataPtr& someData, size_t& outSize)
{
	aTriggeringNodeInstance->FetchData(outType, someData, outSize, aPinIndex);
}
