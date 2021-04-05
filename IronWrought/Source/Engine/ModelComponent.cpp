#include "stdafx.h"
#include "ModelComponent.h"

#include "ModelFactory.h"
#include "GameObject.h"
#include "Model.h"
#include "MaterialHandler.h"

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
	myEmissive = Vector4(1.0f);

	Texture1(ASSETPATH("Assets/Graphics/Textures/Shared/orchid0.dds"));
	Texture2(ASSETPATH("Assets/Graphics/Textures/Shared/orchid0.dds"));
	//Texture3(ASSETPATH("Assets/Graphics/Textures/Shared/blue.dds"));
	Texture4(ASSETPATH("Assets/Graphics/Textures/Shared/orchid2.dds"));
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

bool CModelComponent::SetTints(std::vector<Vector4>& aVectorWithTints)
{
	assert(!aVectorWithTints.empty());
	if (aVectorWithTints.empty())
		return false;

	memmove(&myTints[0].myColor, &aVectorWithTints[0], sizeof(Vector4));
	memmove(&myTints[1].myColor, &aVectorWithTints[1], sizeof(Vector4));
	memmove(&myTints[2].myColor, &aVectorWithTints[2], sizeof(Vector4));
	memmove(&myTints[3].myColor, &aVectorWithTints[3], sizeof(Vector4));

	return true;
}

std::vector<Vector4> CModelComponent::GetTints()
{
	if (myTints.empty())
		myTints.resize(NUMBER_OF_TINT_SLOTS);
	std::vector<Vector4> tints(NUMBER_OF_TINT_SLOTS);
	memcpy(&tints[0], &myTints[0].myColor, sizeof(Vector4));
	memcpy(&tints[1], &myTints[1].myColor, sizeof(Vector4));
	memcpy(&tints[2], &myTints[2].myColor, sizeof(Vector4));
	memcpy(&tints[3], &myTints[3].myColor, sizeof(Vector4));
	return std::move(tints);
}

void CModelComponent::Tint1(const Vector4& aTint)
{
	if(!myTints.empty())
		myTints[0].myColor = aTint;
}
void CModelComponent::Tint2(const Vector4& aTint)
{
	if(!myTints.empty())
		myTints[1].myColor = aTint;
}
void CModelComponent::Tint3(const Vector4& aTint)
{
	if(!myTints.empty())
		myTints[2].myColor = aTint;
}
void CModelComponent::Tint4(const Vector4& aTint)
{
	if(!myTints.empty())
		myTints[3].myColor = aTint;
}

void CModelComponent::Emissive(const Vector4& aTint)
{
	myEmissive = aTint;
}

const Vector4& CModelComponent::Tint1() const
{
	if (myTints.empty())
	{
		assert(false && "myTints are empty!");
		return myEmissive;
	}

	return myTints[0].myColor;
}
const Vector4& CModelComponent::Tint2() const
{
	if (myTints.empty())
	{
		assert(false && "myTints are empty!");
		return myEmissive;
	}

	return myTints[1].myColor;
}
const Vector4& CModelComponent::Tint3() const
{
	if (myTints.empty())
	{
		assert(false && "myTints are empty!");
		return myEmissive;
	}

	return myTints[2].myColor;
}
const Vector4& CModelComponent::Tint4() const
{
	if (myTints.empty())
	{
		assert(false && "myTints are empty!");
		return myEmissive;
	}

	return myTints[3].myColor;
}

const Vector4& CModelComponent::Emissive() const
{
	return myEmissive;
}

 ID3D11ShaderResourceView* CModelComponent::Texture1()
{
	return myTints[0].myTexture.ShaderResource();
}

ID3D11ShaderResourceView* CModelComponent::Texture2() 
{
	return myTints[1].myTexture.ShaderResource();
}

 ID3D11ShaderResourceView* CModelComponent::Texture3() 
{
	return myTints[2].myTexture.ShaderResource();
}

 ID3D11ShaderResourceView* CModelComponent::Texture4() 
{
	return myTints[3].myTexture.ShaderResource();
}

 ID3D11ShaderResourceView* CModelComponent::TextureOnIndex(const int& anIndex)
{
	const size_t index = anIndex < 0 ? 0 : anIndex >= myTints.size() ? myTints.size() - 1 : anIndex;
	return myTints[index].myTexture.ShaderResource();
}

 ID3D11ShaderResourceView* const* CModelComponent::ConstTintTexture1()
 {
	 return myTints[0].myTexture.ConstShaderResource();
 }

 ID3D11ShaderResourceView* const* CModelComponent::ConstTintTexture2()
 {
	 return myTints[1].myTexture.ConstShaderResource();
 }

 ID3D11ShaderResourceView* const* CModelComponent::ConstTintTexture3()
 {
	 return myTints[2].myTexture.ConstShaderResource();
 }

 ID3D11ShaderResourceView* const* CModelComponent::ConstTintTexture4()
 {
	 return myTints[3].myTexture.ConstShaderResource();
 }

 ID3D11ShaderResourceView* const* CModelComponent::ConstTintTextureOnIndex(const int& anIndex)
 {
	 const size_t index = anIndex < 0 ? 0 : anIndex >= myTints.size() ? myTints.size() - 1 : anIndex;
	 return myTints[index].myTexture.ConstShaderResource();
 }

void CModelComponent::Texture1(const std::string& aTexturePath)
{
	myTints[0].myTexture.Init(aTexturePath);
}

void CModelComponent::Texture2(const std::string & aTexturePath)
{
	myTints[1].myTexture.Init(aTexturePath);
}

void CModelComponent::Texture3(const std::string & aTexturePath)
{
	myTints[2].myTexture.Init(aTexturePath);
}

void CModelComponent::Texture4(const std::string & aTexturePath)
{
	myTints[3].myTexture.Init(aTexturePath);
}

void CModelComponent::TextureOnIndex(const std::string& aTexturePath, const int anIndex)
{
	const size_t index = anIndex < 0 ? 0 : anIndex >= myTints.size() ? myTints.size() - 1 : anIndex;
	myTints[index].myTexture.Init(aTexturePath);
}

void CModelComponent::Texture1(ID3D11ShaderResourceView* aTexture)
{
	myTints[0].myTexture.ShaderResource(aTexture);
}

void CModelComponent::Texture2(ID3D11ShaderResourceView* aTexture)
{
	myTints[1].myTexture.ShaderResource(aTexture);
}

void CModelComponent::Texture3(ID3D11ShaderResourceView* aTexture)
{
	myTints[2].myTexture.ShaderResource(aTexture);
}

void CModelComponent::Texture4(ID3D11ShaderResourceView* aTexture)
{
	myTints[3].myTexture.ShaderResource(aTexture);
}

void CModelComponent::TextureOnIndex(ID3D11ShaderResourceView* aTexture, const int anIndex)
{
	const size_t index = anIndex < 0 ? 0 : anIndex >= myTints.size() ? myTints.size() - 1 : anIndex;
	myTints[index].myTexture.ShaderResource(aTexture);
}

bool CModelComponent::SetTintTextures(std::vector<CModelComponent::STintData>& aVectorWithTintTextures)
{
	myTints = std::move(aVectorWithTintTextures);
	return true;
}

const std::vector<CModelComponent::STintData>& CModelComponent::GetTintData()
{
	return myTints;
}

std::array<ID3D11ShaderResourceView* const*, NUMBER_OF_TINT_SLOTS> CModelComponent::GetConstTintTextures()
{
	std::array<ID3D11ShaderResourceView* const*, NUMBER_OF_TINT_SLOTS>  resources = {
		ConstTintTexture1(),
		ConstTintTexture2(),
		ConstTintTexture3(),
		ConstTintTexture4()
	};

	return std::move(resources);
}

std::array<ID3D11ShaderResourceView*, NUMBER_OF_TINT_SLOTS> CModelComponent::GetTintTextures()
{
	std::array<ID3D11ShaderResourceView*, NUMBER_OF_TINT_SLOTS>  resources = {
		Texture1(),
		Texture2(),
		Texture3(),
		Texture4()
	};

	return std::move(resources);
}

void CModelComponent::HasTintMap(const bool aHasTintMap)
{
	if(aHasTintMap)
		if (myTints.empty())
			myTints.resize(NUMBER_OF_TINT_SLOTS);
}

std::array<bool, NUMBER_OF_TINT_SLOTS> CModelComponent::HasTintTexturesOnSlots() const
{
	std::array<bool, NUMBER_OF_TINT_SLOTS> onSlots = { 
		myTints[0].myTexture.HasTexture(),
		myTints[1].myTexture.HasTexture(),
		myTints[2].myTexture.HasTexture(),
		myTints[3].myTexture.HasTexture() 
	};

	return std::move(onSlots);
}
