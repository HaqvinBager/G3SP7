#include "stdafx.h"
#include "MaterialHandler.h"
#include "Engine.h"
#include "DirectXFramework.h"
#include "BinReader.h"
#include "JsonReader.h"
#include "ModelFactory.h"
#include "GraphicsHelpers.h"
#include <unordered_map>
#include "FolderUtility.h"

std::array<ID3D11ShaderResourceView*, 3> CMaterialHandler::RequestMaterial(const std::string& aMaterialName)
{
	if (myMaterials.find(aMaterialName) == myMaterials.end())
	{
		std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3> newTextures;
		newTextures[0] = Graphics::GetShaderResourceView(myDevice, myMaterialPath + aMaterialName + "/" + aMaterialName + "_c.dds");
		newTextures[1] = Graphics::GetShaderResourceView(myDevice, myMaterialPath + aMaterialName + "/" + aMaterialName + "_m.dds");
		newTextures[2] = Graphics::GetShaderResourceView(myDevice, myMaterialPath + aMaterialName + "/" + aMaterialName + "_n.dds");

		myMaterials.emplace(aMaterialName, std::move(newTextures));
		myMaterialReferences.emplace(aMaterialName, 0);
	}

	myMaterialReferences[aMaterialName] += 1;
	std::array<ID3D11ShaderResourceView*, 3> textures;
	textures[0] = myMaterials[aMaterialName][0].Get();
	textures[1] = myMaterials[aMaterialName][1].Get();
	textures[2] = myMaterials[aMaterialName][2].Get();
	return textures;
}

std::array<ID3D11ShaderResourceView*, 3> CMaterialHandler::RequestDecal(const std::string& aDecalName)
{
	if (myMaterials.find(aDecalName) == myMaterials.end())
	{
		std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3> newTextures;
		newTextures[0] = Graphics::TryGetShaderResourceView(myDevice, myDecalPath + aDecalName + "/" + aDecalName + "_c.dds");
		newTextures[1] = Graphics::TryGetShaderResourceView(myDevice, myDecalPath + aDecalName + "/" + aDecalName + "_m.dds");
		newTextures[2] = Graphics::TryGetShaderResourceView(myDevice, myDecalPath + aDecalName + "/" + aDecalName + "_n.dds");

		myMaterials.emplace(aDecalName, std::move(newTextures));
		myMaterialReferences.emplace(aDecalName, 0);
	}

	myMaterialReferences[aDecalName] += 1;
	std::array<ID3D11ShaderResourceView*, 3> textures;
	textures[0] = myMaterials[aDecalName][0].Get();
	textures[1] = myMaterials[aDecalName][1].Get();
	textures[2] = myMaterials[aDecalName][2].Get();
	return textures;
}

std::array<ID3D11ShaderResourceView*, 9> CMaterialHandler::GetVertexPaintMaterials(const std::vector<std::string>& someMaterialNames)
{
	std::array<ID3D11ShaderResourceView*, 9> textures = {};
	for (unsigned int i = 0; i < someMaterialNames.size(); ++i)
	{
		textures[0 + (i * 3)] = myMaterials[someMaterialNames[i]][0].Get();
		textures[1 + (i * 3)] = myMaterials[someMaterialNames[i]][1].Get();
		textures[2 + (i * 3)] = myMaterials[someMaterialNames[i]][2].Get();
	}

	return textures;
}

ID3D11ShaderResourceView* CMaterialHandler::RequestTintMap(const std::string& aTintMapPath)
{
	if (myTintMaps.find(aTintMapPath) == myTintMaps.end())
	{
		std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 1> newTextures;
		newTextures[0] = Graphics::GetShaderResourceView(myDevice, myMaterialPath + aTintMapPath + "/" + aTintMapPath + "_t.dds");

		myTintMaps.emplace(aTintMapPath, std::move(newTextures));
		myTintMapReferences.emplace(aTintMapPath, 0);
	}

	myTintMapReferences[aTintMapPath] += 1;
	ID3D11ShaderResourceView* texture;
	texture = myTintMaps[aTintMapPath][0].Get();

	return texture;
}

ID3D11ShaderResourceView* CMaterialHandler::RequestSingleTexture(const std::string& aTextureName)
{
	if (mySingleTextures.find(aTextureName) == mySingleTextures.end())
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newTexture;
		newTexture = Graphics::GetShaderResourceView(myDevice, mySingleTexturesPath + aTextureName + ".dds");

		mySingleTextures.emplace(aTextureName, std::move(newTexture));
		mySingleTextureReferences.emplace(aTextureName, 0);
	}

	mySingleTextureReferences[aTextureName] += 1;
	ID3D11ShaderResourceView* texture = nullptr;
	texture = mySingleTextures[aTextureName].Get();

	return texture;
}

void CMaterialHandler::ReleaseMaterial(const std::string& aMaterialName)
{
	if (myMaterials.find(aMaterialName) != myMaterials.end())
	{
		myMaterialReferences[aMaterialName] -= 1;

		if (myMaterialReferences[aMaterialName] <= 0)
		{
			ULONG remainingRefs = 0;
			do
			{
				if (myMaterials[aMaterialName][0].Get())
					myMaterials[aMaterialName][0].Get()->Release();
				if (myMaterials[aMaterialName][1].Get())
					myMaterials[aMaterialName][1].Get()->Release();
				if (myMaterials[aMaterialName][2].Get())
					remainingRefs = myMaterials[aMaterialName][2].Get()->Release();
				else
					remainingRefs = 0;
			} while (remainingRefs > 1);

			myMaterials.erase(aMaterialName);
			myMaterialReferences.erase(aMaterialName);
		}
	}
}

void CMaterialHandler::ReleaseTintMap(const std::string& aMaterialName)
{
	if (myTintMaps.find(aMaterialName) != myTintMaps.end())
	{
		myTintMapReferences[aMaterialName] -= 1;

		if (myTintMapReferences[aMaterialName] <= 0)
		{
			ULONG remainingRefs = 0;
			do
			{
				if (myTintMaps[aMaterialName][0].Get())
					remainingRefs = myTintMaps[aMaterialName][0].Get()->Release();
				else
					remainingRefs = 0;
			} while (remainingRefs > 1);

			myTintMaps.erase(aMaterialName);
			myTintMapReferences.erase(aMaterialName);
		}
	}
}

void CMaterialHandler::ReleaseSingleTexture(const std::string& aSingleTextureName)
{
	if (mySingleTextures.find(aSingleTextureName) != mySingleTextures.end())
	{
		mySingleTextureReferences[aSingleTextureName] -= 1;

		if (mySingleTextureReferences[aSingleTextureName] <= 0)
		{
			ULONG remainingRefs = 0;
			do
			{
				if (mySingleTextures[aSingleTextureName].Get())
					remainingRefs = mySingleTextures[aSingleTextureName].Get()->Release();
				else
					remainingRefs = 0;
			} while (remainingRefs > 1);

			mySingleTextures.erase(aSingleTextureName);
			mySingleTextureReferences.erase(aSingleTextureName);
		}
	}
}

SVertexPaintData CMaterialHandler::RequestVertexColorID(int aGameObjectID, const std::string& aFbxModelPath)
{
	std::vector<std::string> jsonPaths = CFolderUtility::GetFileNamesInFolder(myVertexLinksPath, ".json", "ColorCollection");
	SVertexPaintColorData colorData{ {}, {}, 0 };
	std::vector<std::string> materialNames;
	for (auto& jsonPath : jsonPaths)
	{
		rapidjson::Document vertexLinks = CJsonReader::Get()->LoadDocument(myVertexLinksPath + jsonPath);

		if (vertexLinks.HasMember("links"))
		{
			if (vertexLinks["links"].IsArray())
			{
				auto linksArray = vertexLinks["links"].GetArray();
				
				if (linksArray.Empty())
					continue;

				if (!linksArray[0].HasMember("myTransformIDs"))
					continue;
				
				for (unsigned int i = 0; i < linksArray.Size(); ++i)
				{
					for (const auto& gameObjectID : linksArray[i]["myTransformIDs"].GetArray())
					{
						if (gameObjectID.GetInt() == aGameObjectID)
						{
							colorData = CBinReader::LoadVertexColorData(ASSETPATH(linksArray[i]["colorsPath"].GetString()));

							std::vector<Vector3>& fbxVertexPositions = CModelFactory::GetInstance()->GetVertexPositions(aFbxModelPath);					
							std::unordered_map<Vector3, Vector3, CMaterialHandler::VectorHasher, VertexPositionComparer> vertexPositionToColorMap;

							const float epsilon = 0.001f;
							for (const auto& ourFBXVertexPosition : fbxVertexPositions) {
								vertexPositionToColorMap[ourFBXVertexPosition] = { .0f, .0f, .0f };								
							}

							for (unsigned int j = 0; j < colorData.myVertexPositions.size(); ++j) {						
								vertexPositionToColorMap[{ -colorData.myVertexPositions[j].x, colorData.myVertexPositions[j].y, -colorData.myVertexPositions[j].z}] = colorData.myColors[j];						
							}

							std::vector<Vector3> rgbColorData = {};
							for (auto& ourFBXVertexPositionValue : fbxVertexPositions) {
								rgbColorData.push_back(vertexPositionToColorMap[ourFBXVertexPositionValue]);
							}


							if (myVertexColorBuffers.find(colorData.myVertexMeshID) == myVertexColorBuffers.end())
							{
								D3D11_BUFFER_DESC vertexColorBufferDesc;
								vertexColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
								vertexColorBufferDesc.ByteWidth = sizeof(rgbColorData[0]) * static_cast<UINT>(rgbColorData.size());

								vertexColorBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
								vertexColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
								vertexColorBufferDesc.MiscFlags = 0;
								vertexColorBufferDesc.StructureByteStride = 0;
								D3D11_SUBRESOURCE_DATA subResourceData = { 0 };
								subResourceData.pSysMem = rgbColorData.data();

								ID3D11Buffer* vertexColorBuffer;
								ENGINE_HR_MESSAGE(CEngine::GetInstance()->myFramework->GetDevice()->CreateBuffer(&vertexColorBufferDesc, &subResourceData, &vertexColorBuffer), "Vertex Color Buffer could not be created.");

								myVertexColorBuffers.emplace(colorData.myVertexMeshID, std::move(vertexColorBuffer));
								myVertexColorReferences.emplace(colorData.myVertexMeshID, 0);
							}

							myVertexColorReferences[colorData.myVertexMeshID] += 1;

							for (const auto& materialName : linksArray[i]["myMaterialNames"].GetArray())
							{
								RequestMaterial(materialName.GetString());
								materialNames.emplace_back(materialName.GetString());
							}
						}
					}
				}
			}
		}

	}
	return { materialNames, static_cast<unsigned int>(colorData.myVertexMeshID) };
}

std::vector<DirectX::SimpleMath::Vector3>& CMaterialHandler::GetVertexColors(unsigned int aVertexColorID)
{
	return myVertexColors[aVertexColorID];
}

ID3D11Buffer* CMaterialHandler::GetVertexColorBuffer(unsigned int aVertexColorID)
{
	return myVertexColorBuffers[aVertexColorID];
}

void CMaterialHandler::ReleaseVertexColors(unsigned int aVertexColorID)
{
	if (myVertexColorBuffers.find(aVertexColorID) != myVertexColorBuffers.end())
	{
		myVertexColorReferences[aVertexColorID] -= 1;

		if (myVertexColorReferences[aVertexColorID] <= 0)
		{
			ULONG remainingRefs = 0;
			do
			{
				remainingRefs = myVertexColorBuffers[aVertexColorID]->Release();
			} while (remainingRefs > 1);

			myVertexColorBuffers.erase(aVertexColorID);
			myVertexColorReferences.erase(aVertexColorID);
		}
	}
}

bool CMaterialHandler::Init(CDirectXFramework* aFramwork)
{
	myDevice = aFramwork->GetDevice();

	if (!myDevice)
	{
		return false;
	}

	return true;
}

CMaterialHandler::CMaterialHandler()
	: myDevice(nullptr)
	, myMaterialPath(ASSETPATH("Assets/Graphics/Textures/Materials/"))
	, myDecalPath(ASSETPATH("Assets/Graphics/Textures/Decals/"))
	, myVertexLinksPath(ASSETPATH("Assets/Generated/"))
	, mySingleTexturesPath(ASSETPATH("Assets/Graphics/Textures/Shared/"))
{
}

CMaterialHandler::~CMaterialHandler()
{
}