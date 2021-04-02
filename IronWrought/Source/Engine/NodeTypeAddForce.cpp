#include "stdafx.h"
#include "NodeTypeAddForce.h"
#include "NodeInstance.h"

CNodeTypeAddForce::CNodeTypeAddForce()
{
	myPins.push_back(SPin("IN", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFlow));
	myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));		//2

	myPins.push_back(SPin("Force", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EFloat));
	myPins.push_back(SPin("Direction", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EVector3));

	myPins.push_back(SPin("OUT", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EFlow));
}

int CNodeTypeAddForce::OnEnter(CNodeInstance* /*aTriggeringNodeInstance*/)
{
	return 0;
}
