#include "stdafx.h"
#include "InstancedModelComponent.h"
#include "Model.h"
#include "ModelFactory.h"

CInstancedModelComponent::CInstancedModelComponent(CGameObject& aParent, std::string aModelPath, const std::vector<DirectX::SimpleMath::Matrix>& aInstancedTransforms, bool /*aRenderWithAlpha*/)
	: CBehaviour(aParent)
	, myModel(CModelFactory::GetInstance()->GetInstancedModel(aModelPath, static_cast<int> (aInstancedTransforms.size())))
	, myIntancedTransforms(aInstancedTransforms)
	, myModelPath(aModelPath)
{
	myRenderWithAlpha = false;
	std::vector<std::string> materialNames = myModel->GetModelInstanceData().myMaterialNames;
	for (auto& materialName : materialNames)
	{
		if (materialName.substr(materialName.size() - 6, 2) == "AL")
		{
			myRenderWithAlpha = true;
			break;
		}
	}
}

CInstancedModelComponent::~CInstancedModelComponent()
{
	//SAFE_DELETE(myModel);
	CModelFactory::GetInstance()->ClearModel(myModelPath, static_cast<int>(myIntancedTransforms.size()));
}

void CInstancedModelComponent::Awake()
{
}

void CInstancedModelComponent::Start()
{
}

void CInstancedModelComponent::Update()
{
}

void CInstancedModelComponent::OnEnable()
{
}

void CInstancedModelComponent::OnDisable()
{
}
