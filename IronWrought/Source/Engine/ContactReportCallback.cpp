#include "stdafx.h"
#include "ContactReportCallback.h"
#include "TransformComponent.h"
#include <PlayerControllerComponent.h>
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"

void CContactReportCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
{
	actors;
	count;
	//for (physx::PxU32 actorIndex = 0; actorIndex < count; actorIndex++)
	//{
	//	const std::string* actorName = static_cast<std::string*>(actors[actorIndex]->userData);
	//	std::cout << (*actorName) << " woke up and begun exploring the world" << std::endl;
	//}
}

void CContactReportCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
{
	actors;
	count;
	//for (physx::PxU32 actorIndex = 0; actorIndex < count; actorIndex++)
	//{
	//	const std::string* actorName = static_cast<std::string*>(actors[actorIndex]->userData);
	//	std::cout << (*actorName) << " fell asleep and dreamt" << std::endl;
	//}
}

void CContactReportCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	(pairs);
	(count);
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

void CContactReportCallback::onContact(const physx::PxContactPairHeader& /*pairHeader*/, const physx::PxContactPair* /*pairs*/, physx::PxU32 /*nbPairs*/)
{
	// Walls don't uses userData. Only feedback when hamsters collide
	//if (pairHeader.actors[0]->userData != nullptr && pairHeader.actors[1]->userData != nullptr)
	//{
		// Implement what is to happen when two objects collide
		//const std::string* firstRodent = static_cast<std::string*>(pairHeader.actors[0]->userData);
		//const std::string* secondRodent = static_cast<std::string*>(pairHeader.actors[1]->userData);
		
		/*CTransformComponent* firstTransform = (CTransformComponent*)pairHeader.actors[0]->userData;
		CTransformComponent* secondTransform = (CTransformComponent*)pairHeader.actors[1]->userData;
		CPlayerControllerComponent* player = nullptr;
		if (firstTransform->GetComponent<CPlayerControllerComponent>()) {
			player = firstTransform->GetComponent<CPlayerControllerComponent>();
		}
		else if (secondTransform->GetComponent<CPlayerControllerComponent>()) {
			player = secondTransform->GetComponent<CPlayerControllerComponent>();
		}*/

		//if (otherTransform->GameObject().GetComponent<CRigidBodyComponent>()) {
		//	
		//		CPlayerControllerComponent* player = playerTransform->GameObject().GetComponent<CPlayerControllerComponent>();
		//		Vector3 v = player->GetLinearVelocity();
		//		CRigidBodyComponent* other = otherTransform->GetComponent<CRigidBodyComponent>();
		//		float m = other->GetMass();
		//		Vector3 f = { v * (m / CTimer::Dt()) };
		//		other->AddForce(f);
		//		//F = m * (v - v0/t - t0) or F = m * (v/t) because v0 and t0 is almost always 0 in this case
		//		//m = mass
		//		//v = velocity
		//		//t = time
		//	
		//}
	//	std::cout << (*firstRodent) << " puffed " << (*secondRodent) << std::endl;
	//}
}
