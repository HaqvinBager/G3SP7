#include "stdafx.h"
#include "GameObject.h"
#include "Component.h"
#include "TransformComponent.h"
#include "Behaviour.h"

CGameObject::CGameObject(const int aInstanceID) 
	: myInstanceID(aInstanceID)
	, myIsStatic(false)
{
	myComponents.push_back(std::make_unique<CTransformComponent>(*this));
	myTransform = static_cast<CTransformComponent*>(myComponents.back().get());
	//myTransform = std::make_unique<CTransformComponent>();// std::make_unique<CTransformComponent>(*this);	
	myIsActive = true;
}

CGameObject::~CGameObject()
{
	//for (size_t i = 0; i < myComponents.size(); ++i)
	//{
	//	//delete myComponents[i];
	//	//myComponents[i] = nullptr;
	//	myComponents[i].release();
	//}
	myTransform = nullptr;
	myComponents.clear();
}

void CGameObject::Awake()
{

	for (size_t i = 0; i < myComponents.size(); ++i)
	{
		myComponents[i]->Awake();
	}

	for (size_t i = 0; i < myComponents.size(); ++i) {
		myComponents[i]->OnEnable();
	}
}

void CGameObject::Start()
{
	for (size_t i = 0; i < myComponents.size(); ++i)
	{
		if (CBehaviour* behaviour = dynamic_cast<CBehaviour*>(myComponents[i].get()))
		{
			if (behaviour->Enabled()) {
				myComponents[i]->Start();
			}
		}
		else {
			myComponents[i]->Start();
		}
	}
}

void CGameObject::Update()
{
	for (size_t i = 0; i < myComponents.size(); ++i)
	{
		if (CBehaviour* behaviour = dynamic_cast<CBehaviour*>(myComponents[i].get()))
		{
			if (behaviour->Enabled()) {
				myComponents[i]->Update();
			}
		}
		else {
			myComponents[i]->Update();
		}
	}
}

void CGameObject::FixedUpdate()
{
	for (size_t i = 0; i < myComponents.size(); ++i)
	{
		myComponents[i]->FixedUpdate();
	}
}

void CGameObject::LateUpdate()
{
	for (size_t i = 0; i < myComponents.size(); ++i)
	{
		myComponents[i]->LateUpdate();
	}
}

void CGameObject::Collided(CGameObject& aCollidedGameObject)
{
	for (size_t i = 0; i < myComponents.size(); ++i)
	{
		if (CBehaviour* behaviour = dynamic_cast<CBehaviour*>(myComponents[i].get()))
		{
			if (behaviour->Enabled()) {
				myComponents[i]->Collided(&aCollidedGameObject);
			}
		} else {
			myComponents[i]->Collided(&aCollidedGameObject);
		}
	}
}

void CGameObject::Active(bool aActive)
{
	myIsActive = aActive;

	if (aActive) {
		for (size_t i = 0; i < myComponents.size(); ++i) {
			myComponents[i]->OnEnable();
		}
	}
	else {
		for (size_t i = 0; i < myComponents.size(); ++i) {
			myComponents[i]->OnDisable();
		}
	}
}
