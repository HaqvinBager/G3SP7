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
	/*myBehaviours.push_back(new CPatrol(
		{
			{ 20.0f,0.0f, 0.0f },
			{ 20.0f,0.0f, 10.0f },
			{ 0.0f,0.0f, 0.0f }
		}));*/

	CSeek* seekBehaviour = new CSeek();
	if(myPlayer != nullptr)
		seekBehaviour->SetTarget(myPlayer->myTransform);
	//myBehaviours.push_back(seekBehaviour);

	//myBehaviours.push_back(new CAttack());

	this->GameObject().GetComponent<CVFXSystemComponent>()->EnableEffect(0);

	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		GameObject().GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
		GameObject().GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
		GameObject().GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	}
	myCurrentHealth = mySettings.myHealth;
}

void CEnemyComponent::Update()//f�r best�mma vilket behaviour vi vill k�ra i denna Update()!!!
{
	/*myDistance = sqrt(
		(myPlayer->myTransform->Position().x - GameObject().myTransform->Position().x) * ((myPlayer->myTransform->Position().x - GameObject().myTransform->Position().x)) +
		((myPlayer->myTransform->Position().y - GameObject().myTransform->Position().y) * ((myPlayer->myTransform->Position().y - GameObject().myTransform->Position().y))) +
		((myPlayer->myTransform->Position().z - GameObject().myTransform->Position().z) * ((myPlayer->myTransform->Position().z - GameObject().myTransform->Position().z))));*/
	if(myPlayer)
		mySettings.myDistance = Vector3::DistanceSquared(myPlayer->myTransform->Position(), GameObject().myTransform->Position());

	/*Vector3 newPos;
	newPos.x = (float)myController->GetController().getFootPosition().x;
	newPos.y = (float)myController->GetController().getFootPosition().y;
	newPos.z = (float)myController->GetController().getFootPosition().z;*/
	//GameObject().myTransform->Position(GameObject().GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetPosition());

	if (mySettings.myRadius * mySettings.myRadius >= mySettings.myDistance) {//seek
		//SetState(EBehaviour::Seek);
	float attackDistance = 2.0f; //example will probably be more complicated in the future;
		if (mySettings.myDistance <= attackDistance) {
		 //	SetState(EBehaviour::Attack);
			/*TakeDamage();*/
		}
	}
	else {//patrol
		SetState(EBehaviour::Patrol);
	}
	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		GameObject().GetComponent<CRigidBodyComponent>()->AddForce({ 1.f, 0.f, 0.f });
	}
	if (myCurrentHealth <= 0.f) {
		Dead();
		//std::cout << "DEAD" << std::endl;
	}
	//Vector3 newDirection = myBehaviours[static_cast<int>(myCurrentState)]->Update(GameObject().myTransform->Position());
	//myController->Move(newDirection, mySettings.mySpeed);
	//GameObject().myTransform->Position(myController->GetPosition());
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
