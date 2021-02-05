//#include "stdafx.h"
//#include "RigidBodyComponent.h"
//#include "PhysXWrapper.h"
//#include "RigidDynamicBody.h"
//
//
//CRigidBodyComponent::CRigidBodyComponent(CGameObject& aParent, bool isStatic) 
//	: CComponent(aParent)
//{
//	if (isStatic == false) {
//		myDynamicRigidBody = CMainSingleton::PhysXWrapper().CreateDynamicRigidbody(aParent.GetComponent<CTransformComponent>()->Position());
//	}
//	else {
//		myDynamicRigidBody = nullptr;
//	}
//}
//
//CRigidBodyComponent::~CRigidBodyComponent()
//{
//	myDynamicRigidBody = nullptr;
//}
//
//void CRigidBodyComponent::Awake()
//{
//}
//
//void CRigidBodyComponent::Start()
//{
//}
//
//void CRigidBodyComponent::Update()
//{
//	GameObject().myTransform->Position(myDynamicRigidBody->GetPosition());
//}
