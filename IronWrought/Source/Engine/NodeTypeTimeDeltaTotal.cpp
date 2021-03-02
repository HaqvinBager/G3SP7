#include "stdafx.h"
#include "NodeTypeTimeDeltaTotal.h"
#include "NodeInstance.h"

CNodeTypeTimeDeltaTotal::CNodeTypeTimeDeltaTotal()
{
    myPins.emplace_back(SPin("Delta Time", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat)); //0
    myPins.emplace_back(SPin("Total Time", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat)); //1
}

int CNodeTypeTimeDeltaTotal::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
    float deltaTime = CTimer::Dt();
    float totalTime = CTimer::Time();

    std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();

    DeclareDataOnPinIfNecessary<float>(pins[0]);
    memcpy(pins[0].myData, &deltaTime, sizeof(float));

    DeclareDataOnPinIfNecessary<float>(pins[1]);
    memcpy(pins[1].myData, &totalTime, sizeof(float));

    return -1;
}
