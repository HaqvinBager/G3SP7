#include "stdafx.h"
#include "Component.h"
#include "GameObject.h"
#include "TransformComponent.h"

CComponent::CComponent(CGameObject& aParent)
	: myParent(aParent)
{}

CComponent::~CComponent()
{
}

void CComponent::Collided(CGameObject* /*aCollidedGameObject*/)
{

}

CGameObject& CComponent::GameObject() const
{
	return myParent;
}

CTransformComponent* CComponent::Transform()
{
	return myParent.myTransform;
}
