#include "stdafx.h"
#include "TransformComponent.h"
#include "AIController.h"

CPatrol::CPatrol(const std::vector<Vector3>& somePositions)
{
	myPositions = somePositions;
	myTarget = 0;
}

Vector3 CPatrol::Update(Vector3 aPosition)
{

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
	return direction;
}

bool CPatrol::CheckIfOverlap(Vector3 aFirstPosition, Vector3 aSecondPosition)
{
	//float y = aFirstPosition.y - 0.85f;
	float xDifference = fabs(aFirstPosition.x - aSecondPosition.x);
	//float yDifference = fabs(y - aSecondPosition.y);
	float zDifference = fabs(aFirstPosition.z - aSecondPosition.z);
	if (xDifference > 0.01f)
		return false;
	//if (yDifference > 0.01f)
		//return false;
	if (zDifference > 0.01f)
		return false;

	return true;
}

CSeek::CSeek()
{
}

Vector3 CSeek::Update(Vector3 aPosition)//aPostion == EnemyRobot Position
{
	Vector3 direction = myTarget->Position() - aPosition;
	return direction;
}

void CSeek::SetTarget(CTransformComponent* aTarget) {
	myTarget = aTarget;
}

CAttack::CAttack() : myDamage(1.0f) {
}

CAttack::~CAttack(){
}

Vector3 CAttack::Update(Vector3 /*aPosition*/)
{

	return Vector3();
}

void CAttack::DoAttack() {

}