#include "stdafx.h"
#include "MaterialHandler.h"
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
				myMaterials[aMaterialName][0].Get()->Release();
				myMaterials[aMaterialName][1].Get()->Release();
				remainingRefs = myMaterials[aMaterialName][2].Get()->Release();
			} while (remainingRefs > 1);

			myMaterials.erase(aMaterialName);
			myMaterialReferences.erase(aMaterialName);
		}
	}
}

unsigned int CMaterialHandler::RequestVertexColorID(int aGameObjectID)
{
	SVertexPaintColorData colorData{ {}, 0 };
	rapidjson::Document vertexLinks = CJsonReader::LoadDocument(myVertexLinksPath);
	if (vertexLinks.HasMember("links"))
	{
		if (vertexLinks["links"].IsArray())
		{
			auto linksArray = vertexLinks["links"].GetArray();
			for (unsigned int i = 0; i < linksArray.Size(); ++i)
			{
				if (linksArray[i]["myGameObjectIDs"][0] == aGameObjectID)
				{
					colorData = CBinReader::LoadVertexColorData(ASSETPATH + linksArray[i]["colorsPath"].GetString());

					if (myVertexColors.find(colorData.myVertexMeshID) == myVertexColors.end())
					{
						myVertexColors.emplace(colorData.myVertexMeshID, colorData.myColors);
						myVertexColorReferences.emplace(colorData.myVertexMeshID, 0);
					}

					myVertexColorReferences[colorData.myVertexMeshID] += 1;
				}
			}
		}
	}
	return colorData.myVertexMeshID;
}

std::vector<DirectX::SimpleMath::Vector3>& CMaterialHandler::GetVertexColors(unsigned int aVertexColorID)
{
	return myVertexColors[aVertexColorID];
}

void CMaterialHandler::ReleaseVertexColors(unsigned int aVertexColorID)
{
	if (myVertexColors.find(aVertexColorID) != myVertexColors.end())
	{
		myVertexColorReferences[aVertexColorID] -= 1;

		if (myVertexColorReferences[aVertexColorID] <= 0)
		{
			myVertexColors.erase(aVertexColorID);
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
