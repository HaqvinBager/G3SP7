#include "stdafx.h"
#include "EnemyReportCallback.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"
#include <EnemyComponent.h>

void CEnemyReportCallback::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	CTransformComponent* enemy = (CTransformComponent*)hit.controller->getUserData();
	if (enemy) {
		if (enemy->GetComponent<CEnemyComponent>()) {
				//CTransformComponent* objectTransform = (CTransformComponent*)hit.actor->userData;
				//if (objectTransform) {
				//	CRigidBodyComponent* other = objectTransform->GetComponent<CRigidBodyComponent>();
				//	if (other) {
						//enemy->GetComponent<CEnemyComponent>()->TakeDamage();
				//	}
				//}
		
		}
	}
}

void CEnemyReportCallback::onControllerHit(const physx::PxControllersHit& /*hit*/)
{
}

void CEnemyReportCallback::onObstacleHit(const physx::PxControllerObstacleHit& /*hit*/)
{
}
