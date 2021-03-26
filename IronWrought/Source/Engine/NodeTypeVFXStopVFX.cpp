#include "stdafx.h"
#include "NodeTypeVFXStopVFX.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "GameObject.h"
#include "GraphManager.h"
#include "VFXSystemComponent.h"

CNodeTypeVFXStopVFX::CNodeTypeVFXStopVFX()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
	myPins.push_back(SPin("VFX Name", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EString));
	//DeclareDataOnPinIfNecessary<const char*>(myPins[1]);
}

int CNodeTypeVFXStopVFX::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject()[0];

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	std::string name = static_cast<char*>(someData);

	auto component = gameObject->GetComponent<CVFXSystemComponent>();
	if (component != nullptr)
		gameObject->GetComponent<CVFXSystemComponent>()->DisableEffect(name);

	return -1;
}
