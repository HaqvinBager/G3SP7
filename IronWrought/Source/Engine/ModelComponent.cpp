#include "stdafx.h"
#include "ModelComponent.h"
#include "ModelFactory.h"
#include "GameObject.h"
#include "Model.h"
#include "MaterialHandler.h"

CModelComponent::CModelComponent(CGameObject& aParent, const std::string& aFBXPath) : CBehaviour(aParent) {
	myModel = CModelFactory::GetInstance()->GetModel(aFBXPath);
	myModelPath = aFBXPath;

	SVertexPaintData vertexPaintData = CMainSingleton::MaterialHandler().RequestVertexColorID(aParent.InstanceID());
	myVertexPaintColorID = vertexPaintData.myVertexColorID;
	myVertexPaintMaterialNames = vertexPaintData.myRGBMaterialNames;
}

CModelComponent::~CModelComponent()
{
	CModelFactory::GetInstance()->ClearModel(myModelPath);

	for (const auto& vertexPaintMaterialName : myVertexPaintMaterialNames)
	{
		CMainSingleton::MaterialHandler().ReleaseMaterial(vertexPaintMaterialName);
	}
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

const unsigned int CModelComponent::VertexPaintColorID() const 
{
	return myVertexPaintColorID;
}

const std::vector<std::string>& CModelComponent::VertexPaintMaterialNames() const
{
	return myVertexPaintMaterialNames;
}