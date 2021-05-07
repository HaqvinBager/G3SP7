#include "stdafx.h"
#include "EnemyComponent.h"
#include "CharacterController.h"
#include "AIController.h"
#include "TransformComponent.h"
#include "VFXSystemComponent.h"
#include <Scene.h>
#include "Engine.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"
#include "CapsuleColliderComponent.h"
#include "RigidBodyComponent.h"
#include "ModelComponent.h"
#include "PhysXWrapper.h"

//EnemyComp

CEnemyComponent::CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings)
	: CComponent(aParent)
	, myPlayer(nullptr)
	, myEnemy(nullptr)
	, myCurrentState(EBehaviour::Count)
	, myRigidBodyComponent(nullptr)
{
	//myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(GameObject().myTransform->Position(), 0.6f * 0.5f, 1.8f * 0.5f, GameObject().myTransform, aHitReport);
	//myController->GetController().getActor()->setRigidBodyFlag(PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
	mySettings = someSettings;
	myPitch = 0.0f;
	myYaw = 0.0f;
}

CEnemyComponent::~CEnemyComponent()
{
	myRigidBodyComponent = nullptr;
	for (size_t i = 0; i < myBehaviours.size(); ++i)
	{
		delete myBehaviours[i];
	}
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

	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		myRigidBodyComponent = GameObject().GetComponent<CRigidBodyComponent>();
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true); 
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(mySettings.mySpeed);
	}
	myCurrentHealth = mySettings.myHealth;
}

void CEnemyComponent::Update()//får bestämma vilket behaviour vi vill köra i denna Update()!!!
{
	float distanceToPlayer = Vector3::DistanceSquared(myPlayer->myTransform->Position(), GameObject().myTransform->Position());

	if (mySettings.myRadius * mySettings.myRadius >= distanceToPlayer) {
		if (distanceToPlayer <= mySettings.myAttackDistance * mySettings.myAttackDistance) 
		{
			SetState(EBehaviour::Attack);
		}
		else
		{
			SetState(EBehaviour::Seek);
		}
	}
	else {
		SetState(EBehaviour::Patrol);
	}

	if (myRigidBodyComponent) {
		Vector3 targetDirection = myBehaviours[static_cast<int>(myCurrentState)]->Update(GameObject().myTransform->Position());
		
		targetDirection.y = 0;
		myRigidBodyComponent->AddForce(targetDirection,mySettings.mySpeed);
		float targetOrientation = WrapAngle(atan2f(targetDirection.x, targetDirection.z));
		myCurrentOrientation = Lerp(myCurrentOrientation, targetOrientation, 2.0f * CTimer::Dt());
		GameObject().myTransform->Rotation({ 0, DirectX::XMConvertToDegrees(myCurrentOrientation) + 180.f, 0 });
	}

//new movement
	//if (GameObject().GetComponent<CRigidBodyComponent>()) {
//		GameObject().GetComponent<CRigidBodyComponent>()->AddForce({ 1.f, 0.f, 0.f });
	//}
	if (myCurrentHealth <= 0.f) {
		Dead();
	}
}

void CEnemyComponent::FixedUpdate()
{
	//myController->Move({ 0.0f, -0.098f, 0.0f }, 1.f);
}

void CEnemyComponent::TakeDamage(float aDamage)
{
	myCurrentHealth -= aDamage;
}

void CEnemyComponent::SetState(EBehaviour aState)
{
	if (myCurrentState == aState)
		return;

	myCurrentState = aState;

	EMessageType msgType = EMessageType::Count;
	switch (myCurrentState)
	{
		case EBehaviour::Patrol:
		{
			//std::cout << __FUNCTION__ << " " << "PATROL" << std::endl;
			msgType = EMessageType::EnemyPatrolState;
		}break;

		case EBehaviour::Seek:
		{
			//std::cout << __FUNCTION__ << " " << "SEEK" << std::endl;
			msgType = EMessageType::EnemySeekState;
		}break;

		case EBehaviour::Attack:
		{
			//std::cout << __FUNCTION__ << " " << "ATTACK" << std::endl;
			msgType = EMessageType::EnemyAttackState;
		}break;


		default:
		break;
	}
	if (msgType == EMessageType::Count)
		return;
	CMainSingleton::PostMaster().SendLate({ msgType, this });
}

const CEnemyComponent::EBehaviour CEnemyComponent::GetState() const
{
	return myCurrentState;
}

void CEnemyComponent::Dead()
{
	GameObject().Active(false);
}
