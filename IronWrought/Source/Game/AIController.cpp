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

CAttack::CAttack() : myDamage(1.0f), myTarget(nullptr), myAttackCooldown(1.f), myAttackTimer(0.f) {
}

Vector3 CAttack::Update(const Vector3& aPosition)
{
	if (!myTarget) {
		return Vector3();
	}
	Vector3 direction = myTarget->WorldPosition() - aPosition;

	//byt ut attacktimer och attackcooldown till animationtimer - Alexander Matthäi 2021-05-07
	myAttackTimer += CTimer::Dt();
	if (myAttackTimer >= myAttackCooldown) {
		Vector3 origin = aPosition;
		PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(origin, direction, 10.0f, CPhysXWrapper::ELayerMask::PLAYER);
		int hits = hit.getNbAnyHits();

		/*CLineInstance* myLine2 = new CLineInstance();
		myLine2->Init(CLineFactory::GetInstance()->CreateLine(origin, origin + (direction * 10.f), { 255,0,0,255 }));
		CEngine::GetInstance()->GetActiveScene().AddInstance(myLine2);*/

		if (hits > 0) {
			std::cout << "Player Hit " << std::endl;
			float damage = 5.0f;
			CMainSingleton::PostMaster().Send({ EMessageType::PlayerTakeDamage, &damage });
		}
		myAttackTimer = 0.f;
	}
	return std::move(direction);
}

void CAttack::SetTarget(CTransformComponent* aTarget)
{
	myTarget = aTarget;
}