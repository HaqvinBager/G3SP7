#include "stdafx.h"
#include "NodeTypeVFXPlayVFX.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "GameObject.h"
#include "GraphManager.h"
#include "VFXSystemComponent.h"

CNodeTypeVFXPlayVFX::CNodeTypeVFXPlayVFX()
{
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_IN));
	myPins.push_back(SPin("VFX Name", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EString));
	//DeclareDataOnPinIfNecessary<const char*>(myPins[1]);

}

int CNodeTypeVFXPlayVFX::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject();

	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 1, outType, someData, outSize);
	std::string name = static_cast<char*>(someData);

	auto component = gameObject->GetComponent<CVFXSystemComponent>();
	if (component != nullptr)
		gameObject->GetComponent<CVFXSystemComponent>()->EnableEffect(name);

	return -1;
}
