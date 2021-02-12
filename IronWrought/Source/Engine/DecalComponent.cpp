#include "stdafx.h"
#include "DecalComponent.h"
#include "ModelComponent.h"
#include "DecalFactory.h"
#include "GameObject.h"
#include "Decal.h"

CDecalComponent::CDecalComponent(CGameObject& aParent, const std::string& aFBXPath)
    : CBehaviour(aParent)
{
    myDecal = CDecalFactory::GetInstance()->GetDecal(aFBXPath);
    myDecalPath = aFBXPath;
}

CDecalComponent::~CDecalComponent()
{
    CDecalFactory::GetInstance()->ClearDecal(myDecalPath);
}

void CDecalComponent::Awake()
{
}

void CDecalComponent::Start()
{
}

void CDecalComponent::Update()
{
}

CDecal* CDecalComponent::GetMyDecal() const
{
    return myDecal;
}

void CDecalComponent::OnEnable()
{
}

void CDecalComponent::OnDisable()
{
}
