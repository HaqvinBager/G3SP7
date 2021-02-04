#include "stdafx.h"
#include "MaterialHandler.h"

#include "Engine.h"
#include "DirectXFramework.h"
#include "BinReader.h"
#include "JsonReader.h"

std::array<ID3D11ShaderResourceView*, 3> CMaterialHandler::RequestMaterial(const std::string& aMaterialName)
{
	if (myMaterials.find(aMaterialName) == myMaterials.end())
	{
		std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3> newTextures;
		newTextures[0] = GetShaderResourceView(myDevice, myMaterialPath + aMaterialName + "/" + aMaterialName + "_c.dds");
		newTextures[1] = GetShaderResourceView(myDevice, myMaterialPath + aMaterialName + "/" + aMaterialName + "_m.dds");
		newTextures[2] = GetShaderResourceView(myDevice, myMaterialPath + aMaterialName + "/" + aMaterialName + "_n.dds");

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

SVertexPaintData CMaterialHandler::RequestVertexColorID(int aGameObjectID)
{
	std::vector<std::string> jsonPaths = CJsonReader::GetFilePathsInFolder(myVertexLinksPath, "PolybrushLinks_");
	SVertexPaintColorData colorData{ {}, 0 };
	std::vector<std::string> materialNames;
	for (auto& jsonPath : jsonPaths)
	{
		rapidjson::Document vertexLinks = CJsonReader::LoadDocument(myVertexLinksPath + jsonPath);

		if (vertexLinks.HasMember("links"))
		{
			if (vertexLinks["links"].IsArray())
			{
				auto linksArray = vertexLinks["links"].GetArray();
				for (unsigned int i = 0; i < linksArray.Size(); ++i)
				{
					for (const auto& gameObjectID : linksArray[i]["myGameObjectIDs"].GetArray())
					{
						if (gameObjectID.GetInt() == aGameObjectID)
						{
							colorData = CBinReader::LoadVertexColorData(ASSETPATH + linksArray[i]["colorsPath"].GetString());

							//if (myVertexColors.find(colorData.myVertexMeshID) == myVertexColors.end())
							if (myVertexColorBuffers.find(colorData.myVertexMeshID) == myVertexColorBuffers.end())
							{
								//myVertexColors.emplace(colorData.myVertexMeshID, colorData.myColors);
								D3D11_BUFFER_DESC vertexColorBufferDesc;
								vertexColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
								vertexColorBufferDesc.ByteWidth = sizeof(colorData.myColors[0]) * static_cast<UINT>(colorData.myColors.size());
								vertexColorBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
								vertexColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
								vertexColorBufferDesc.MiscFlags = 0;
								vertexColorBufferDesc.StructureByteStride = 0;

								D3D11_SUBRESOURCE_DATA subResourceData = { 0 };
								subResourceData.pSysMem = colorData.myColors.data();

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
	//if (myVertexColors.find(aVertexColorID) != myVertexColors.end())
	//{
	//	myVertexColorReferences[aVertexColorID] -= 1;

	//	if (myVertexColorReferences[aVertexColorID] <= 0)
	//	{
	//		myVertexColors.erase(aVertexColorID);
	//		myVertexColorReferences.erase(aVertexColorID);
	//	}
	//}

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

ID3D11ShaderResourceView* CMaterialHandler::GetShaderResourceView(ID3D11Device* aDevice, const std::string& aTexturePath)
{
	ID3D11ShaderResourceView* shaderResourceView;

	wchar_t* widePath = new wchar_t[aTexturePath.length() + 1];
	std::copy(aTexturePath.begin(), aTexturePath.end(), widePath);
	widePath[aTexturePath.length()] = 0;

	////==ENABLE FOR TEXTURE CHECKING==
	//ENGINE_HR_MESSAGE(DirectX::CreateDDSTextureFromFile(aDevice, widePath, nullptr, &shaderResourceView), aTexturePath.append(" could not be found.").c_str());
	////===============================

	//==DISABLE FOR TEXTURE CHECKING==
	HRESULT result;
	result = DirectX::CreateDDSTextureFromFile(aDevice, widePath, nullptr, &shaderResourceView);
	if (FAILED(result))
	{
		std::string errorTexturePath = aTexturePath.substr(aTexturePath.length() - 6);
		errorTexturePath = ASSETPATH + "Assets/ErrorTextures/Checkboard_128x128" + errorTexturePath;

		wchar_t* wideErrorPath = new wchar_t[errorTexturePath.length() + 1];
		std::copy(errorTexturePath.begin(), errorTexturePath.end(), wideErrorPath);
		wideErrorPath[errorTexturePath.length()] = 0;

		DirectX::CreateDDSTextureFromFile(aDevice, wideErrorPath, nullptr, &shaderResourceView);
		delete[] wideErrorPath;
	}

	delete[] widePath;
	return shaderResourceView;
}

CMaterialHandler::CMaterialHandler()
	: myDevice(nullptr)
{
}

CMaterialHandler::~CMaterialHandler()
{
}
