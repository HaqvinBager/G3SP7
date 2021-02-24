#include "stdafx.h"
#include "NodeTypeDeltaTime.h"
#include "NodeInstance.h"

CNodeTypeDeltaTime::CNodeTypeDeltaTime()
{
    myPins.emplace_back(SPin("Delta Time", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float)); //0
    myPins.emplace_back(SPin("Total Time", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float)); //1
}

int CNodeTypeDeltaTime::OnEnter(CNodeInstance* aTriggeringNodeInstance)
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
