#include "stdafx.h"
#include "CNodeTypeMathSub.h"
#include "CNodeInstance.h"

CNodeTypeMathSub::CNodeTypeMathSub()
{
    myPins.push_back(SPin("Val1", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("Val2", SPin::PinTypeInOut::PinTypeInOut_IN, SPin::PinType::Float));
    myPins.push_back(SPin("OUT", SPin::PinTypeInOut::PinTypeInOut_OUT, SPin::PinType::Float));
}

int CNodeTypeMathSub::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
    SPin::PinType outType;
    NodeDataPtr someData = nullptr;
    size_t outSize = 0;

    GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
    float input1 = NodeData::Get<float>(someData);

    GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
    float input2 = NodeData::Get<float>(someData);

    float temp = input1 - input2;
    std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
    DeclareDataOnPinIfNecessary<float>(pins[2]);
    memcpy(pins[2].myData, &temp, sizeof(float));

    return -1;
}
