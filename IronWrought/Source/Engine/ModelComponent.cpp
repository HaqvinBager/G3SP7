#include "stdafx.h"
#include "ModelComponent.h"
#include "ModelFactory.h"
#include "GameObject.h"
#include "Model.h"

CModelComponent::CModelComponent(CGameObject& aParent, const std::string& aFBXPath) : CBehaviour(aParent) {
	myModel = CModelFactory::GetInstance()->GetModel(aFBXPath);
	myModelPath = aFBXPath;
}

CModelComponent::CModelComponent(CGameObject& aParent, const std::string& anFBXPath, const std::string& /*aVertexColorsPath*/) 
	: CBehaviour(aParent)
{
	myModel = CModelFactory::GetInstance()->GetModel(anFBXPath);
	myModelPath = anFBXPath;


}

CModelComponent::~CModelComponent()
{
	//SAFE_DELETE(myModel);
	CModelFactory::GetInstance()->ClearModel(myModelPath);
}

void CModelComponent::Awake()
{}

void CModelComponent::Start()
{}

void CModelComponent::Update()
{}

void CModelComponent::SetModel(const std::string& aFBXPath)
{
	myModel = CModelFactory::GetInstance()->GetModel(aFBXPath);
}

CModel* CModelComponent::GetMyModel() const
{
	return myModel;
}

void CModelComponent::OnEnable()
{}

void CModelComponent::OnDisable()
{}
