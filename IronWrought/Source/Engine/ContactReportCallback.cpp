#include "stdafx.h"
#include "ContactReportCallback.h"
#include "TransformComponent.h"
#include <PlayerControllerComponent.h>
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"
#include "BoxColliderComponent.h"
#include <EnemyComponent.h>
#include "Engine.h"
#include "Scene.h"
//#include "GameObject.h"

void CContactReportCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
{
	for (physx::PxU32 actorIndex = 0; actorIndex < count; actorIndex++)
	{
		const std::string* actorName = static_cast<std::string*>(actors[actorIndex]->userData);
		std::cout << (*actorName) << " woke up and begun exploring the world" << std::endl;
	}
}

void CContactReportCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
{
	for (physx::PxU32 actorIndex = 0; actorIndex < count; actorIndex++)
	{
		const std::string* actorName = static_cast<std::string*>(actors[actorIndex]->userData);
		std::cout << (*actorName) << " fell asleep and dreamt" << std::endl;
	}
}

void CContactReportCallback::onTriggerEnter(physx::PxActor* trigger, physx::PxActor* other)
{
	CBoxColliderComponent* triggerVolume = (CBoxColliderComponent*)trigger->userData;
	if (triggerVolume != nullptr)
	{
		CTransformComponent* transform = static_cast<CTransformComponent*>(other->userData);
		if (transform != nullptr)
		{
			triggerVolume->OnTriggerEnter(transform);
		}
		else
		{
			transform = CEngine::GetInstance()->GetActiveScene().PlayerController()->GameObject().myTransform;
			triggerVolume->OnTriggerEnter(transform);
		}
	}
}

void CContactReportCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			onTriggerEnter(pairs[i].triggerActor, pairs[i].otherActor);
		}
		else if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
			onTriggerExit(pairs[i].triggerActor, pairs[i].otherActor);
		}
		
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
			physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;
	}
}

void CContactReportCallback::onTriggerExit(physx::PxActor* trigger, physx::PxActor* other)
{
	(trigger);
	(other);
	CBoxColliderComponent* triggerVolume = (CBoxColliderComponent*)trigger->userData;
	if (triggerVolume != nullptr)
	{
		CTransformComponent* transform = static_cast<CTransformComponent*>(other->userData);
		if (transform != nullptr)
		{
			triggerVolume->OnTriggerExit(transform);
		}
		else
		{
			transform = CEngine::GetInstance()->GetActiveScene().PlayerController()->GameObject().myTransform;
			triggerVolume->OnTriggerEnter(transform);
		}
	}
}

void CContactReportCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
	(bodyBuffer);
	(poseBuffer);
	(count);
}

void CContactReportCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{
	(constraints);
	(count);
}

void CContactReportCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* /*pairs*/, physx::PxU32 /*nbPairs*/)
{
	// Walls don't uses userData. Only feedback when hamsters collide
	if (pairHeader.actors[0]->userData != nullptr && pairHeader.actors[1]->userData != nullptr)
	{
		CTransformComponent* firstTransform = (CTransformComponent*)pairHeader.actors[0]->userData;
		CTransformComponent* secondTransform = (CTransformComponent*)pairHeader.actors[1]->userData;
		CEnemyComponent* enemy = nullptr;
		float length = 0;
		if (firstTransform->GetComponent<CEnemyComponent>()) {
			enemy = firstTransform->GetComponent<CEnemyComponent>();
			//check velocity
			if (secondTransform->GetComponent<CRigidBodyComponent>()) {
				length = secondTransform->GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetLinearVelocity().LengthSquared();
				if (length >= 50.f) {
					enemy->TakeDamage(5.f);
				}
			}
		}
		//probably is not needed because it will always be the object that collides with the enemy
		/*else if (secondTransform->GetComponent<CEnemyComponent>()) {
			enemy = secondTransform->GetComponent<CEnemyComponent>();
			if (firstTransform->GetComponent<CRigidBodyComponent>()) {
				length = firstTransform->GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetLinearVelocity().LengthSquared();
				if (length >= 500.f) {
					enemy->TakeDamage();
				}
			}
		}*/
	}
}
