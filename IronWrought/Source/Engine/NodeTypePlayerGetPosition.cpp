#include "stdafx.h"
#include "NodeTypePlayerGetPosition.h"

#include "NodeInstance.h"

#include "Engine.h"
#include "Scene.h"
#include "GameObject.h"
#include "TransformComponent.h"

CNodeTypePlayerGetPosition::CNodeTypePlayerGetPosition()
{
    myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3)); //0
	DeclareDataOnPinIfNecessary<Vector3>(myPins[0], Vector3(0.0f, 0.0f, 0.0f));

}

int CNodeTypePlayerGetPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	const Vector3& position = IRONWROUGHT_ACTIVE_SCENE.Player()->myTransform->Position();

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[0]);
	memcpy(pins[0].myData, &position, sizeof(Vector3));

	return -1;
}
