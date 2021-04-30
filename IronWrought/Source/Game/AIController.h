#pragma once
class CTransformComponent;

class CAIController
{
public:
	virtual ~CAIController() { }
	virtual Vector3 Update(const Vector3& aPosition) = 0; // Aki 28/4 använd gärna const & på vector3 så att man inte kopierar 3 floats i varje update :)
	
};

class CPatrol: public CAIController
{
public:
	
	CPatrol(const std::vector<Vector3>& somePositions);
	~CPatrol()override {}
	Vector3 Update(const Vector3& aPosition) override;
	bool CheckIfOverlap(const Vector3& aFirstPosition, const Vector3& aSecondPosition);
private:
	std::vector<Vector3> myPositions;
	int myTarget;
};

class CSeek: public CAIController 
{
public:
	CSeek();
	~CSeek()override { myTarget = nullptr; }
	Vector3 Update(const Vector3& aPosition) override;
	void SetTarget(CTransformComponent* aTarget);
private:
	CTransformComponent* myTarget;
};

class CAttack : public CAIController
{
public:
	CAttack();
	~CAttack() override { myTarget = nullptr; }
	Vector3 Update(const Vector3& aPosition) override;
	void SetTarget(CTransformComponent* aTarget);

private:
	float myDamage;
	CTransformComponent* myTarget;
};

