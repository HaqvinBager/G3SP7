#include "stdafx.h"
#include "CharacterController.h"
#include "Engine.h"
#include "PhysXWrapper.h"

using namespace physx;

CCharacterController::CCharacterController(const Vector3 aPosition)
{
    PxCapsuleControllerDesc desc;
    desc.position = {aPosition.x, aPosition.y, aPosition.z};
    desc.height = 2.f;
    desc.radius = 1.f;
    desc.material = CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::basic);
    myController = CEngine::GetInstance()->GetPhysx().GetControllerManager().createController(desc);
}

physx::PxController& CCharacterController::GetController()
{
    return *myController;
}

Vector3 CCharacterController::GetPosition() const
{
    const PxExtendedVec3 vec3 = myController->getPosition();
    return {(float)vec3.x, (float)vec3.y, (float)vec3.z};
}
