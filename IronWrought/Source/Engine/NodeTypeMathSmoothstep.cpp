#include "stdafx.h"
#include "NodeTypeMathSmoothstep.h"
#include "NodeInstance.h"

CNodeTypeMathSmoothstep::CNodeTypeMathSmoothstep()
{
    myPins.push_back(SPin("Edge1", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
    myPins.push_back(SPin("Edge2", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
    myPins.push_back(SPin("Val", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
    myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFloat));
}

int CNodeTypeMathSmoothstep::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	float edge1 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	float edge2 = NodeData::Get<float>(someData);

	GetDataOnPin(aTriggeringNodeInstance, 2, outType, someData, outSize);
	float value = NodeData::Get<float>(someData);

	value = (value - edge1) / (edge2 - edge1);

	if (value < 0.0f)
		value = 0.0f;

	if (value > 1.0f)
		value = 1.0f;

	value = value * value * (3 - 2 * value);

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<float>(pins[3]);
	memcpy(pins[3].myData, &value, sizeof(float));

	return -1;
}
