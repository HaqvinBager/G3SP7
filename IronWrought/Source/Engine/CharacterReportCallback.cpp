#include "stdafx.h"
#include "CharacterReportCallback.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include <PlayerControllerComponent.h>
#include "RigidDynamicBody.h"

void CCharacterReportCallback::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	//CTransformComponent* transform = (CTransformComponent*)hit.actor->userData;
	/*CRigidBodyComponent* rigid = transform->GameObject().GetComponent<CRigidBodyComponent>();
	if (rigid) {
		std::cout << rigid->GameObject().InstanceID() << std::endl;
	}*/
	CTransformComponent* playerTransform = (CTransformComponent*)hit.controller->getUserData();
	if(playerTransform->GameObject().GetComponent<CPlayerControllerComponent>()){
		if (hit.shape->getGeometryType() != physx::PxGeometryType::eTRIANGLEMESH) {
			CTransformComponent* objectTransform = (CTransformComponent*)hit.actor->userData;
			CPlayerControllerComponent* player = playerTransform->GameObject().GetComponent<CPlayerControllerComponent>();
			if (player) {
				if (objectTransform) {
					Vector3 v = player->GetLinearVelocity();
					Vector3 n = {hit.worldNormal.x, hit.worldNormal.y, hit.worldNormal.z};
					CRigidBodyComponent* other = objectTransform->GetComponent<CRigidBodyComponent>();
					if (other) {
						float m = other->GetMass();
						//använder normalen istället för velocity för det puttas bättre åt de håll man går in i
						Vector3 f = { (m * (v / CTimer::Dt())) };
						other->AddForce(f);
						//other->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(10.f);
						//F = m * (v - v0/t - t0) or F = m * (v/t) because v0 and t0 is almost always 0 in this case
						//m = mass
						//v = velocity
						//t = time

						std::cout << "YES" << std::endl;
					}
				}
			}
		}
	}
	
}

void CCharacterReportCallback::onControllerHit(const physx::PxControllersHit& /*hit*/)
{
}

void CCharacterReportCallback::onObstacleHit(const physx::PxControllerObstacleHit& /*hit*/)
{
	/*CTransformComponent* transform = (CTransformComponent*)hit.controller->getActor()->userData;
	CRigidBodyComponent* rigid = transform->GameObject().GetComponent<CRigidBodyComponent>();
	if (rigid) {
		std::cout << rigid->GameObject().InstanceID() << std::endl;
	}*/
}
