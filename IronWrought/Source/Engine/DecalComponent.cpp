#include "stdafx.h"
#include "DecalComponent.h"
#include "ModelComponent.h"
#include "ModelFactory.h"
#include "GameObject.h"
#include "Model.h"

CDecalComponent::CDecalComponent(CGameObject& aParent, const std::string& /*aFBXPath*/)
    : CBehaviour(aParent)
{
    myModel = CModelFactory::GetInstance()->GetModel(aFBXPath);
    myModelPath = aFBXPath;
}

CDecalComponent::~CDecalComponent()
{
    CModelFactory::GetInstance()->ClearModel(myModelPath);
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

void CDecalComponent::SetModel(const std::string& /*aFBXPath*/)
{
}

CModel* CDecalComponent::GetMyModel() const
{
    return nullptr;
}

void CDecalComponent::OnEnable()
{
}

void CDecalComponent::OnDisable()
{
}
