#pragma once
class CTransformComponent;

class CAIController
{
public:
	virtual ~CAIController() { }
	virtual Vector3 Update(Vector3 aPosition) = 0;
	
};

class CPatrol: public CAIController
{
public:
	
	CPatrol(const std::vector<Vector3>& somePositions);
	~CPatrol()override {}
	Vector3 Update(Vector3 aPosition) override;
	bool CheckIfOverlap(Vector3 aFirstPosition, Vector3 aSecondPosition);
private:
	std::vector<Vector3> myPositions;
	int myTarget;
};

class CSeek: public CAIController 
{
public:
	CSeek();
	~CSeek()override { myTarget = nullptr; }
	Vector3 Update(Vector3 aPosition) override;
	void SetTarget(CTransformComponent* aTarget);
private:
	CTransformComponent* myTarget;
};

class CAttack : public CAIController
{
public:
	CAttack();
	~CAttack() override { myTarget = nullptr; }
	Vector3 Update(Vector3 aPosition) override;

private:
	float myDamage;
	CTransformComponent* myTarget;
};

