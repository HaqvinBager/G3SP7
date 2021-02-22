#include "stdafx.h"
#include "NodeTypeGetMousePosition.h"
#include "NodeInstance.h"
#include "Input.h"
#include "Engine.h"
#include "WindowHandler.h"

CNodeTypeGetMousePosition::CNodeTypeGetMousePosition()
{
	myPins.push_back(SPin("X", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float));
	myPins.push_back(SPin("Y", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Float));
	myPins.push_back(SPin("Left Down", SPin::EPinTypeInOut::PinTypeInOut_OUT, SPin::EPinType::Bool));
}

int CNodeTypeGetMousePosition::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();

	DeclareDataOnPinIfNecessary<float>(pins[0]);
	float xPosition = static_cast<float>(Input::GetInstance()->MouseX()) / static_cast<float>(CEngine::GetInstance()->GetWindowHandler()->GetResolution().x);
	memcpy(pins[0].myData, &xPosition, sizeof(float));

	DeclareDataOnPinIfNecessary<float>(pins[1]);
	float yPosition = static_cast<float>(Input::GetInstance()->MouseY()) / static_cast<float>(CEngine::GetInstance()->GetWindowHandler()->GetResolution().y);
	memcpy(pins[1].myData, &yPosition, sizeof(float));

	DeclareDataOnPinIfNecessary<bool>(pins[2]);
	bool isDown = Input::GetInstance()->IsMouseDown(Input::EMouseButton::Left);
	memcpy(pins[2].myData, &isDown, sizeof(bool));

	return -1;
}
