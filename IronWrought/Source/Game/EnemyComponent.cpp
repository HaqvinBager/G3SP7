#include "stdafx.h"
#include "EnemyComponent.h"
#include "CharacterController.h"
#include "AIController.h"
#include "TransformComponent.h"
#include "VFXSystemComponent.h"
#include <Scene.h>
#include "Engine.h"
#include "PhysXWrapper.h"

//EnemyComp

CEnemyComponent::CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings, physx::PxUserControllerHitReport* aHitReport)
	: CComponent(aParent)
	, myController(nullptr)
	, myPlayer(nullptr)
	, myEnemy(nullptr)
	, myCurrentState(EBehaviour::Patrol)
{
	mySettings = someSettings;
	myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(GameObject().myTransform->Position(), 0.6f * 0.5f, 1.8f * 0.5f, GameObject().myTransform, aHitReport);
	myPitch = 0.0f;
	myYaw = 0.0f;
}

CEnemyComponent::~CEnemyComponent()
{
}

void CEnemyComponent::Awake()
{
}

void CEnemyComponent::Start()
{
	myPlayer = CEngine::GetInstance()->GetActiveScene().Player();

	for (const auto id : mySettings.myPatrolGameObjectIds) {
		CTransformComponent* patrolTransform = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(id)->myTransform;
		myPatrolPositions.push_back(patrolTransform->Position());
	}
	myBehaviours.push_back(new CPatrol(myPatrolPositions));

	CSeek* seekBehaviour = new CSeek();
	myBehaviours.push_back(seekBehaviour);
	if (myPlayer != nullptr)
	{
		seekBehaviour->SetTarget(myPlayer->myTransform);
	}

	CAttack* attack = new CAttack();
	if(myPlayer != nullptr)
		attack->SetTarget(myPlayer->myTransform);
	myBehaviours.push_back(attack);

	this->GameObject().GetComponent<CVFXSystemComponent>()->EnableEffect(0);
}

void CEnemyComponent::Update()//får bestämma vilket behaviour vi vill köra i denna Update()!!!
{
	float distanceToPlayer = Vector3::DistanceSquared(myPlayer->myTransform->Position(), GameObject().myTransform->Position());

	if (mySettings.myRadius * mySettings.myRadius >= distanceToPlayer) {
		SetState(EBehaviour::Seek);
		if (distanceToPlayer <= mySettings.myAttackDistance * mySettings.myAttackDistance) {
			SetState(EBehaviour::Attack);
		}
	}
	else {
		SetState(EBehaviour::Patrol);
	}

	Vector3 newDirection = myBehaviours[static_cast<int>(myCurrentState)]->Update(GameObject().myTransform->Position()); // current direction

	myYaw = WrapAngle(myYaw + newDirection.x);
	myController->Move(newDirection, mySettings.mySpeed);
	GameObject().myTransform->Rotation({ 0.0f,myYaw,0.0f });
	GameObject().myTransform->Position(myController->GetPosition());
	//GameObject().myTransform->MoveLocal(-newDirection * mySettings.mySpeed * CTimer::Dt());
}

void CEnemyComponent::TakeDamage()
{
	mySettings.myHealth -= 5.0f;

	std::cout << mySettings.myHealth << std::endl;
}

void CEnemyComponent::SetState(EBehaviour aState)
{
	myCurrentState = aState;
}

const CEnemyComponent::EBehaviour CEnemyComponent::GetState() const
{
	return myCurrentState;
}
