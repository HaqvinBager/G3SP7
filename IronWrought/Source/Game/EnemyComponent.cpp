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
#include "ModelComponent.h"
#include "PhysXWrapper.h"

//EnemyComp

CEnemyComponent::CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings, physx::PxUserControllerHitReport* /*aHitReport*/)
	: CComponent(aParent)
	, myController(nullptr)
	, myPlayer(nullptr)
	, myEnemy(nullptr)
	, myCurrentState(EBehaviour::Patrol)
{
	//myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(GameObject().myTransform->Position(), 0.6f * 0.5f, 1.8f * 0.5f, GameObject().myTransform, aHitReport);
	//myController->GetController().getActor()->setRigidBodyFlag(PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
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

	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		GameObject().GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
		GameObject().GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
		GameObject().GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	}
	myCurrentHealth = mySettings.myHealth;
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

	Vector3 targetDirection = myBehaviours[static_cast<int>(myCurrentState)]->Update(GameObject().myTransform->Position());
	myController->Move(targetDirection, mySettings.mySpeed);
	GameObject().myTransform->Position(myController->GetPosition());
	float targetOrientation = WrapAngle(atan2f(targetDirection.x, targetDirection.z));
	myCurrentOrientation = Lerp(myCurrentOrientation, targetOrientation, 2.0f * CTimer::Dt());
	//myCurrentDirection = Vector3::Lerp(myCurrentDirection, targetDirection, CTimer::Dt());
	GameObject().myTransform->Rotation({ 0, DirectX::XMConvertToDegrees(myCurrentOrientation) + 180.f, 0 });

//new movement
	//if (GameObject().GetComponent<CRigidBodyComponent>()) {
//		GameObject().GetComponent<CRigidBodyComponent>()->AddForce({ 1.f, 0.f, 0.f });
	//}
	if (myCurrentHealth <= 0.f) {
		Dead();
		//std::cout << "DEAD" << std::endl;
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
	myCurrentState = aState;
}

const CEnemyComponent::EBehaviour CEnemyComponent::GetState() const
{
	return myCurrentState;
}

void CEnemyComponent::Dead()
{
	GameObject().Active(false);
}
