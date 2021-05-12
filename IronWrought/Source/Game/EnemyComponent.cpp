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
#include "PlayerComponent.h"
#include "RigidBodyComponent.h"
#include "ModelComponent.h"
#include "PhysXWrapper.h"
#include "VFXSystemComponent.h"

//EnemyComp

CEnemyComponent::CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings)
	: CComponent(aParent)
	, myPlayer(nullptr)
	, myEnemy(nullptr)
	, myCurrentState(EBehaviour::Count)
	, myRigidBodyComponent(nullptr)
	, myTakeDamageTimer(0.0f)
	, mySpeedModifier(1.0f)
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

	CAttack* attack = new CAttack(this);
	if(myPlayer != nullptr)
		attack->SetTarget(myPlayer->myTransform);
	myBehaviours.push_back(attack);

	this->GameObject().GetComponent<CVFXSystemComponent>()->EnableEffect(0);

	mySettings.mySpeed = mySettings.mySpeed < 5.0f ? 5.0f : mySettings.mySpeed;
	mySettings.myHealth = mySettings.myHealth < 10.0f ? 10.0f : mySettings.myHealth;

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
	// :grimacing:
	myTakeDamageTimer -= CTimer::Dt();
	if (myCurrentHealth <= 0.f) {

		myTimeToDeathTimer -= CTimer::Dt();
		if (myTimeToDeathTimer <= 0.0f)
		{
			CMainSingleton::PostMaster().SendLate({ EMessageType::EnemyDisabled, this });
			if (myTimeToDeathTimer <= -0.12f)
			{
				Dead();
			}
		}
		else
		{
			CMainSingleton::PostMaster().SendLate({ EMessageType::EnemyDied, this });
			auto vfx = GameObject().GetComponent<CVFXSystemComponent>();
			if(vfx->Enabled())
				CMainSingleton::PostMaster().SendLate({ EMessageType::PlayRobotDeathSound, nullptr } );
			vfx->Enabled(false);
		}
		return;
	}

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
		myRigidBodyComponent->AddForce(targetDirection,mySettings.mySpeed * mySpeedModifier);
		float targetOrientation = WrapAngle(atan2f(targetDirection.x, targetDirection.z));
		myCurrentOrientation = Lerp(myCurrentOrientation, targetOrientation, 2.0f * CTimer::Dt());
		GameObject().myTransform->Rotation({ 0, DirectX::XMConvertToDegrees(myCurrentOrientation) + 180.f, 0 });
	}
}

void CEnemyComponent::FixedUpdate()
{
	//myController->Move({ 0.0f, -0.098f, 0.0f }, 1.f);
}

void CEnemyComponent::TakeDamage(const float& aDamage)
{
	if (myTakeDamageTimer > 0.0f)
		return;

	myTakeDamageTimer = 0.5f;
	myCurrentHealth -= aDamage;
	std::cout << aDamage << std::endl;
	CMainSingleton::PostMaster().SendLate({ EMessageType::EnemyTakeDamage, this });
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
			mySpeedModifier = 1.0f;
		}break;

		case EBehaviour::Seek:
		{
			//std::cout << __FUNCTION__ << " " << "SEEK" << std::endl;
			msgType = EMessageType::EnemySeekState;
			mySpeedModifier = 1.5f;
		}break;

		case EBehaviour::Attack:
		{
			//std::cout << __FUNCTION__ << " " << "ATTACK" << std::endl;
			msgType = EMessageType::EnemyAttackState;
			mySpeedModifier = 1.5f;
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
	float deadPos = static_cast<float>(0xDEAD);
	GameObject().myTransform->Position({ deadPos, deadPos, deadPos });
	myRigidBodyComponent->SetPosition({ deadPos, deadPos, deadPos });
	GameObject().Active(false);
}
