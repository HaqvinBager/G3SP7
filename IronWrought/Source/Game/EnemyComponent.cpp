#include "stdafx.h"
#include "EnemyComponent.h"
#include "CharacterController.h"
#include "AIController.h"
#include "TransformComponent.h"
#include <Scene.h>
#include "Engine.h"
#include "PhysXWrapper.h"

//EnemyComp

CEnemyComponent::CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings)
	: CComponent(aParent)
	, myController(nullptr)
	, myPlayer(nullptr)
	, myCurrentState(EBehaviour::Patrol)
{
	mySettings = someSettings;
	myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(GameObject().myTransform->Position(), 0.6f * 0.5f, 1.8f * 0.5f);
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
	seekBehaviour->SetTarget(myPlayer->myTransform);
	myBehaviours.push_back(seekBehaviour);
	myBehaviours.push_back(new CAttack());
}

void CEnemyComponent::Update()//f�r best�mma vilket behaviour vi vill k�ra i denna Update()!!!
{
	mySettings.myDistance = Vector3::DistanceSquared(myPlayer->myTransform->Position(), GameObject().myTransform->Position());

	if (mySettings.myRadius * mySettings.myRadius >= mySettings.myDistance) {//seek
		SetState(EBehaviour::Seek);
		//float attackDistance = 2.0f; //example will probably be more complicated in the future; 
		//if (mySettings.myDistance <= attackDistance) {
		 //	SetState(EBehaviour::Attack); 
			/*TakeDamage();*/
		//}
	}
	else {//patrol
		SetState(EBehaviour::Patrol);
	}
	Vector3 newDirection = myBehaviours[static_cast<int>(myCurrentState)]->Update(GameObject().myTransform->Position());
	newDirection.Normalize();
	GameObject().myTransform->Rotation(myPatrolRotation.CreateFromYawPitchRoll(newDirection.y, 0.0f, 0.0f));
	myController->Move(newDirection, mySettings.mySpeed);
	GameObject().myTransform->Position(myController->GetPosition());
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