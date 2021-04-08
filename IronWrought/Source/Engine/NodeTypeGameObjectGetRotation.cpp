#include "stdafx.h"
#include "NodeTypeGameObjectGetRotation.h"
#include "NodeInstance.h"
#include "Scene.h"
#include "Engine.h"
#include "GameObject.h"
#include "GraphManager.h"

CNodeTypeGameObjectGetRotation::CNodeTypeGameObjectGetRotation()
{
	myPins.push_back(SPin("Pos", SPin::EPinTypeInOut::EPinTypeInOut_OUT, SPin::EPinType::EVector3));
}

int CNodeTypeGameObjectGetRotation::OnEnter(CNodeInstance* aTriggeringNodeInstance)
{
	CGameObject* gameObject = IRONWROUGHT_ACTIVE_SCENE.FindObjectWithID(aTriggeringNodeInstance->GraphManager()->GetCurrentBlueprintInstanceID());
	Quaternion quaternionRotation = gameObject->myTransform->Rotation();
    Vector3 eulerRotation;

    //copy from Wikipedia
    float sinr_cosp = 2 * (quaternionRotation.w * quaternionRotation.x + quaternionRotation.y * quaternionRotation.z);
    float cosr_cosp = 1 - 2 * (quaternionRotation.x * quaternionRotation.x + quaternionRotation.y * quaternionRotation.y);
    eulerRotation.x = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = 2 * (quaternionRotation.w * quaternionRotation.y - quaternionRotation.z * quaternionRotation.x);
    if (std::abs(sinp) >= 1)
        eulerRotation.y = std::copysign(3.141592f / 2.0f, sinp); // use 90 degrees if out of range
    else
        eulerRotation.y = std::asin(sinp);

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (quaternionRotation.w * quaternionRotation.z + quaternionRotation.x * quaternionRotation.y);
    float cosy_cosp = 1 - 2 * (quaternionRotation.y * quaternionRotation.y + quaternionRotation.z * quaternionRotation.z);
    eulerRotation.z = std::atan2(siny_cosp, cosy_cosp);

    eulerRotation *= 180.0f / 3.141592f;

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[0]);
	memcpy(pins[0].myData, &eulerRotation, sizeof(Vector3));

	return -1;
}