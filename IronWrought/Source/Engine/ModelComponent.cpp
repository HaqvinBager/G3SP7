#include "stdafx.h"
#include "ModelComponent.h"

#include "ModelFactory.h"
#include "GameObject.h"
#include "Model.h"
#include "MaterialHandler.h"

#include "JsonReader.h"
#include <fstream>
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include <time.h>

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

	TintTextureOnIndex(ASSETPATH("Assets/Graphics/Textures/Shared/orchid0.dds"), 0);
	TintTextureOnIndex(ASSETPATH("Assets/Graphics/Textures/Shared/orchid0.dds"), 1);
	//TintTextureOnIndex(ASSETPATH("Assets/Graphics/Textures/Shared/blue.dds"), 2);
	TintTextureOnIndex(ASSETPATH("Assets/Graphics/Textures/Shared/orchid2.dds"), 3);
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
	myModelPath = aFBXPath;

	SVertexPaintData vertexPaintData = CMainSingleton::MaterialHandler().RequestVertexColorID(GameObject().InstanceID(), aFBXPath);
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

void CModelComponent::TintOnIndex(const Vector4& aTint, const int& anIndex)
{
	if (!myTints.empty())
	{
		const int index = TintsBoundsCheck(anIndex);
		myTints[index].myColor = aTint;
	}
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

const Vector4& CModelComponent::TintOnIndex(const int& anIndex)
{
	const int index = TintsBoundsCheck(anIndex);
	return myTints[index].myColor;
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

 ID3D11ShaderResourceView* CModelComponent::TintTextureOnIndex(const int& anIndex)
{
	const int index = TintsBoundsCheck(anIndex);
	return myTints[index].myTexture.ShaderResource();
}
 ID3D11ShaderResourceView* const* CModelComponent::ConstTintTextureOnIndex(const int& anIndex)
 {
	 const int index = TintsBoundsCheck(anIndex);
	 return myTints[index].myTexture.ConstShaderResource();
 }

void CModelComponent::TintTextureOnIndex(const std::string& aTexturePath, const int& anIndex)
{
	const int index = TintsBoundsCheck(anIndex);
	myTints[index].myTexture.Init(aTexturePath);
}
void CModelComponent::TintTextureOnIndex(ID3D11ShaderResourceView* aTexture, const int& anIndex)
{
	const int index = TintsBoundsCheck(anIndex);
	myTints[index].myTexture.ShaderResource(aTexture);
}

const std::string& CModelComponent::TextureNameOnIndex(const int& anIndex)
{
	const int index = TintsBoundsCheck(anIndex);
	return myTints[index].myTexture.TextureName();
}

const std::string& CModelComponent::TexturePathOnIndex(const int& anIndex)
{
	const int index = TintsBoundsCheck(anIndex);
	return myTints[index].myTexture.TexturePath();
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
		ConstTintTextureOnIndex(0),
		ConstTintTextureOnIndex(1),
		ConstTintTextureOnIndex(2),
		ConstTintTextureOnIndex(3)
	};
	return std::move(resources);
}

std::array<ID3D11ShaderResourceView*, NUMBER_OF_TINT_SLOTS> CModelComponent::GetTintTextures()
{
	std::array<ID3D11ShaderResourceView*, NUMBER_OF_TINT_SLOTS>  resources = {
		TintTextureOnIndex(0),
		TintTextureOnIndex(1),
		TintTextureOnIndex(2),
		TintTextureOnIndex(3)
	};
	return std::move(resources);
}

void CModelComponent::HasTintMap(const bool aHasTintMap)
{
	if(aHasTintMap)
		if (myTints.empty())
			myTints.resize(NUMBER_OF_TINT_SLOTS);
}

#define TINTED_MODEL_DATA_PATH ASSETPATH("Assets/Graphics/TintedModels/Data/")
namespace cModelComponent_jsonKeys
{
	// Should not be std::string.
	// Re-used keys:
	constexpr char keyRed[4]		 = "Red";
	constexpr char keyGreen[6]		 = "Green";
	constexpr char keyBlue[5]		 = "Blue";
	constexpr char keyAlpha[6]		 = "Alpha";
	constexpr char keyEmissive[9]	 = "Emissive";
	constexpr char keyTints[6]		 = "Tints";
	constexpr char keyTexture[8]	 = "Texture";
	constexpr char keyModelPath[11]  = "Model Path";
}
bool CModelComponent::SerializeTintData(const std::string& aTintDataPath)
{
#ifndef NDEBUG
	std::string aFileName;
	if (aTintDataPath.empty())
	{
		aFileName = TINTED_MODEL_DATA_PATH;
		aFileName.append("TintData");

		time_t rawTime = time(0);
		struct tm timeInfo;
		time(&rawTime);
		localtime_s(&timeInfo, &rawTime);
		char timeBuffer[18];// YYYY-MM-DD-hhmmss == 17 + '\0' = 18
		ZeroMemory(timeBuffer, 18);
		strftime(timeBuffer, 18, "%F-%H%M%S", &timeInfo);//%F gets YYYY-MM-DD, %H gets hours: 00->24, %M gets minutes: 00->59, %S gets seconds: 00->59. 

		aFileName.append(timeBuffer);
		aFileName.append(".json");
	}
	else
	{
		aFileName = aTintDataPath;
	}

	// These are only used for information inside the json.
	std::array<std::string, NUMBER_OF_TINT_SLOTS> tintKeys = {
		"Tint1-Primary-Red Channel",
		"Tint2-Secondary-Green Channel",
		"Tint3-Tertiary-Blue Channel",
		"Tint4-Accents-Alpha Channel"
	};

	using namespace rapidjson;
	using namespace cModelComponent_jsonKeys;
	StringBuffer stringBuffer;
	PrettyWriter<StringBuffer> prettyWriter(stringBuffer);
	prettyWriter.StartObject();
	{
		prettyWriter.Key(keyModelPath);
		prettyWriter.String(myModelPath.c_str());

		prettyWriter.Key(keyTints);
		prettyWriter.StartArray();
		{
			for (short i = 0; i < NUMBER_OF_TINT_SLOTS; ++i)
			{
				// Could be made into a Serialize function in CModelComponent
				prettyWriter.StartObject();
				{
					prettyWriter.Key(tintKeys[i].c_str()); // Seems like rapidjson::Objects can't have a key inside of a rapidjson::Array.. ?
					prettyWriter.String("");
					const Vector4& tint = myTints[i].myColor;
					prettyWriter.Key(keyRed);
					prettyWriter.Double(static_cast<double>(tint.x));
					prettyWriter.Key(keyGreen);
					prettyWriter.Double(static_cast<double>(tint.y));
					prettyWriter.Key(keyBlue);
					prettyWriter.Double(static_cast<double>(tint.z));
					prettyWriter.Key(keyAlpha);
					prettyWriter.Double(static_cast<double>(tint.w));

					prettyWriter.Key(keyTexture);
					prettyWriter.String(myTints[i].myTexture.TexturePath().c_str());
				}
				prettyWriter.EndObject();
			}
		}
		prettyWriter.EndArray();

		prettyWriter.Key(keyEmissive); 
		prettyWriter.StartObject();
		{
			prettyWriter.Key(keyRed);
			prettyWriter.Double(static_cast<double>(myEmissive.x));
			prettyWriter.Key(keyGreen);
			prettyWriter.Double(static_cast<double>(myEmissive.y));
			prettyWriter.Key(keyBlue);
			prettyWriter.Double(static_cast<double>(myEmissive.z));
			prettyWriter.Key(keyAlpha);
			prettyWriter.Double(static_cast<double>(myEmissive.w));
		}
		prettyWriter.EndObject();
	}
	prettyWriter.EndObject();

	std::ofstream outFileStream(aFileName);
	outFileStream << stringBuffer.GetString();
	outFileStream.close();
#endif
	return true;
}

bool CModelComponent::DeserializeTintData(const std::string & aTintDataPath)
{
	using namespace rapidjson;
	using namespace cModelComponent_jsonKeys;
	Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);

	myModelPath = document.HasMember(keyModelPath) ? document[keyModelPath].GetString() : "";

	assert(document[keyTints].IsArray() && std::string(std::string(keyTints) + " is not an array in: " + aTintDataPath).c_str());
	if (!document[keyTints].IsArray())
		return false;

	GenericArray tintsArray = document[keyTints].GetArray();
	for (short i = 0; i < static_cast<short>(tintsArray.Size()); ++i)
	{
		GenericObject tintObject = tintsArray[i].GetObjectW();
		myTints[i].myColor.x = tintObject.HasMember(keyRed)   ? tintObject[keyRed].GetFloat()   : 1.0f;
		myTints[i].myColor.y = tintObject.HasMember(keyGreen) ? tintObject[keyGreen].GetFloat() : 1.0f;
		myTints[i].myColor.z = tintObject.HasMember(keyBlue)  ? tintObject[keyBlue].GetFloat()  : 1.0f;
		myTints[i].myColor.w = tintObject.HasMember(keyAlpha) ? tintObject[keyAlpha].GetFloat() : 1.0f;

		if (tintObject.HasMember(keyTexture))
		{
			std::string texturePath = tintObject[keyTexture].GetString();
			if (texturePath.length() > 0)
				myTints[i].myTexture.Init(texturePath);
		}
	}

	assert(document[keyEmissive].IsObject() && std::string( std::string(keyEmissive) + " is not an object in: " + aTintDataPath).c_str());
	if (!document[keyEmissive].IsObject())
		return false;

	auto emissiveObject = document[keyEmissive].GetObjectW();
	myEmissive.x = emissiveObject.HasMember(keyRed)   ? emissiveObject[keyRed].GetFloat()   : 0.0f;
	myEmissive.y = emissiveObject.HasMember(keyGreen) ? emissiveObject[keyGreen].GetFloat() : 0.0f;
	myEmissive.z = emissiveObject.HasMember(keyBlue)  ? emissiveObject[keyBlue].GetFloat()  : 0.0f;
	myEmissive.w = emissiveObject.HasMember(keyAlpha) ? emissiveObject[keyAlpha].GetFloat() : 0.0f;

	return true;
}

int CModelComponent::TintsBoundsCheck(const int& anIndex)
{
	int index = static_cast<int>(anIndex < 0 ? 0 : anIndex >= myTints.size() ? myTints.size() - 1 : anIndex);
	return std::move(index);
}
