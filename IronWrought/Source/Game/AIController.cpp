#include "stdafx.h"
#include "TransformComponent.h"
#include "AIController.h"
#include "PhysXWrapper.h"
#include "PlayerComponent.h"
#include "LineInstance.h"
#include "LineFactory.h"
#include "Scene.h"


CPatrol::CPatrol(const std::vector<Vector3>& somePositions)
{
	myPositions = somePositions;
	myTarget = 0;
}

Vector3 CPatrol::Update(const Vector3& aPosition)
{
	if (myPositions.empty())
		return Vector3::Zero;

	if (CheckIfOverlap(aPosition, myPositions[myTarget]))
	{
		myTarget++;
	}
	if (myTarget >= myPositions.size())
	{
		myTarget = 0;
	}
	Vector3 direction = myPositions[myTarget] - aPosition;

	direction.Normalize();
	return std::move(direction);
}

bool CPatrol::CheckIfOverlap(const Vector3& aFirstPosition, const Vector3& aSecondPosition)
{
	float xDifference = fabs(aFirstPosition.x - aSecondPosition.x);
	float zDifference = fabs(aFirstPosition.z - aSecondPosition.z);
	if (xDifference > 0.1f)
		return false;
	if (zDifference > 0.1f)
		return false;
	return true;
}

CSeek::CSeek() :myTarget(nullptr)
{

}

Vector3 CSeek::Update(const Vector3& aPosition)//aPostion == EnemyRobot Position
{
	if (!myTarget)
		return Vector3();

	Vector3 direction = myTarget->Position() - aPosition;
	direction.Normalize();

	return std::move(direction);
}

void CSeek::SetTarget(CTransformComponent* aTarget) {
	myTarget = aTarget;
}

CAttack::CAttack() : myDamage(1.0f), myTarget(nullptr) {
}

Vector3 CAttack::Update(const Vector3& aPosition)
{
	if (!myTarget) {
		return Vector3();
	}

	Vector3 direction = myTarget->Position() - aPosition;
	direction.Normalize();
	PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(aPosition + Vector3{0.0f,0.0f,1.0f}, direction, 10.0f);
	int hits = hit.getNbAnyHits();

	//CLineInstance* myLine2 = new CLineInstance();
	//myLine2->Init(CLineFactory::GetInstance()->CreateLine(aPosition + Vector3{ 0.0f,0.0f,1.0f }, aPosition + (direction * 5.f), { 255,0,0,255 }));
	//CEngine::GetInstance()->GetActiveScene().AddInstance(myLine2);
	
	if (hits > 0) {
		CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
		if (transform) {
			std::cout << __FUNCTION__ << " Instance ID: " << transform->GameObject().InstanceID() << std::endl;
			if (transform->GetComponent<CPlayerComponent>()) {
				float damage = 5.0f;
				CMainSingleton::PostMaster().Send({ EMessageType::PlayerTakeDamage, &damage });
			}
		}
	}
	return std::move(direction);
}

void CAttack::SetTarget(CTransformComponent* aTarget)
{
	myTarget = aTarget;
}
