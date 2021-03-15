#include "stdafx.h"
#include "CharacterController.h"
#include "PhysXWrapper.h"
#include "Engine.h"

using namespace physx;

CCharacterController::CCharacterController(const Vector3 aPosition, const float aRadius, const float aHeight)
{
    PxCapsuleControllerDesc desc;
    desc.position = {aPosition.x, aPosition.y, aPosition.z};
    desc.height = aHeight;
    desc.radius = aRadius;
    desc.material = CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::basic);
    myController = CEngine::GetInstance()->GetPhysx().GetControllerManager()->createController(desc);
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

//CCharacterController::CCharacterController(PxControllerShapeType::Enum aType, const Vector3& aPos, const float& aRadius, const float& aHeight)
//{
//	if (aType == PxControllerShapeType::eCAPSULE) {
//		InitCapsuelController(aPos, aRadius, aHeight);
//	}
//	else if (aType == PxControllerShapeType::eBOX) {
//
//	}
//	else {
//		myController = nullptr;
//	}
//}
//
//CCharacterController::~CCharacterController()
//{
//	myController = nullptr;
//}
//
//bool CCharacterController::InitCapsuelController(const Vector3& aPos, const float& aRadius, const float& aHeight)
//{
//	PxCapsuleControllerDesc desc;
//	desc.position = PxExtendedVec3({aPos.x, aPos.y, aPos.z});
//	desc.radius = aRadius;
//	desc.height = aHeight;
//	//desc.behaviorCallback;
//	//desc.climbingMode;
//	desc.contactOffset = 0.05f;
//	//desc.density;
//	//desc.invisibleWallHeight;
//	constexpr physx::PxReal staticFriction = 0.7f;
//	constexpr physx::PxReal dynamicFriction = 0.9f;
//	constexpr physx::PxReal restitution = 0.9f;
//	physx::PxMaterial* material = CEngine::GetInstance()->GetPhysx().GetPhysics()->createMaterial(staticFriction, dynamicFriction, restitution);
//	desc.material = material;
//	//desc.maxJumpHeight;
//	//desc.reportCallback;
//	desc.slopeLimit = 0.6f;
//	desc.stepOffset = 0.5f;
//	desc.upDirection = PxVec3(0.f, 1.f, 0.f);
//	//desc.userData;
//	//desc.volumeGrowth;
//	if (!desc.isValid()) {
//		return false;
//	}
//	myController = CEngine::GetInstance()->GetPhysx().GetControllerManger()->createController(desc);
//	if (!myController) {
//		return false;
//	}
//	return true;
//}
//
//Vector3 CCharacterController::GetPosition() const {
//	Vector3 vec;
//	vec.x = static_cast<float>(myController->getPosition().x);
//	vec.y = static_cast<float>(myController->getPosition().y);
//	vec.z = static_cast<float>(myController->getPosition().z);
//	return vec;
//}
