#include "stdafx.h"
#include "ModelComponent.h"
#include "ModelFactory.h"
#include "GameObject.h"
#include "Model.h"
#include "MaterialHandler.h"
#include "Model.h"

CModelComponent::CModelComponent(CGameObject& aParent, const std::string& aFBXPath) : CBehaviour(aParent) {
	myModel = CModelFactory::GetInstance()->GetModel(aFBXPath);
	myModelPath = aFBXPath;

	SVertexPaintData vertexPaintData = CMainSingleton::MaterialHandler().RequestVertexColorID(aParent.InstanceID(), aFBXPath);
	myVertexPaintColorID = vertexPaintData.myVertexColorID;
	myVertexPaintMaterialNames = vertexPaintData.myRGBMaterialNames;

	myRenderWithAlpha = false;
	std::vector<std::string> materialNames = myModel->GetModelData().myMaterialNames;
	for (auto& materialName : materialNames)
	{
		if (materialName.substr(materialName.size() - 2, 2) == "AL")
		{
			myRenderWithAlpha = true;
			break;
		}
	}

	HasTintMap(myModel->GetModelData().myTintMap != nullptr);
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

void CModelComponent::Tint1(const Vector3& aTint)
{
	if(!myTints.empty())
		myTints[0] = aTint;
}
void CModelComponent::Tint2(const Vector3& aTint)
{
	if(!myTints.empty())
		myTints[1] = aTint;
}
void CModelComponent::Tint3(const Vector3& aTint)
{
	if(!myTints.empty())
		myTints[2] = aTint;
}
void CModelComponent::Tint4(const Vector3& aTint)
{
	if(!myTints.empty())
		myTints[3] = aTint;
}

Vector4 CModelComponent::Tint1() const
{
	if (myTints.empty())
		return Vector4();
	Vector4 v = {myTints[0].x, myTints[0].y, myTints[0].z, 1.0f };
	return std::move(v);
}
Vector4 CModelComponent::Tint2() const
{
	if (myTints.empty())
		return Vector4();
	Vector4 v = {myTints[1].x, myTints[1].y, myTints[1].z, 1.0f };
	return std::move(v);
}
Vector4 CModelComponent::Tint3() const
{
	if (myTints.empty())
		return Vector4();
	Vector4 v = { myTints[2].x, myTints[2].y, myTints[2].z, 1.0f };
	return std::move(v);
}
Vector4 CModelComponent::Tint4() const
{
	if (myTints.empty())
		return Vector4();
	Vector4 v = { myTints[3].x, myTints[3].y, myTints[3].z, 1.0f };
	return std::move(v);
}

void CModelComponent::HasTintMap(const bool aHasTintMap)
{
	if(aHasTintMap)
		if(myTints.empty())
			myTints.resize(4);
}