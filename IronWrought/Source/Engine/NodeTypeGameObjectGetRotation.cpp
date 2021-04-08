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
	Quaternion quaternion = gameObject->myTransform->Rotation();
    Vector3 rotation;
    Matrix transform = Matrix::CreateFromQuaternion(quaternion);

    const float RD_TO_DEG = 180 / 3.141592f;
    //float h, p, b; // angles in degrees

    // extract pitch
    float sinP = -transform._23;
    if (sinP >= 1) {
        rotation.x = 90;
    }       // pole
    else if (sinP <= -1) {
        rotation.x = -90;
    } // pole
    else {
        rotation.x = asinf(sinP) * RD_TO_DEG;
    }

    // extract heading and bank
    if (sinP < -0.9999 || sinP > 0.9999) { // account for small angle errors
        rotation.y = atan2f(-transform._31, transform._11) * RD_TO_DEG;
        rotation.z = 0;
    }
    else {
        rotation.y = atan2f(transform._13, transform._33) * RD_TO_DEG;
        rotation.z = atan2f(transform._21, transform._22) * RD_TO_DEG;
    }

    //r *= 360.0f / 3.141592f;

	std::vector<SPin>& pins = aTriggeringNodeInstance->GetPins();
	DeclareDataOnPinIfNecessary<Vector3>(pins[0]);
	memcpy(pins[0].myData, &rotation, sizeof(Vector3));

	return -1;
}