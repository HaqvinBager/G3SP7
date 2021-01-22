#include "stdafx.h"
#include "MaterialHandler.h"

std::array<ID3D11ShaderResourceView*, 3> CMaterialHandler::RequestMaterial(const std::string aMaterialName)
{
	if (myMaterials.find(aMaterialName) == myMaterials.end())
	{
		std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3> newTextures;
		newTextures[0] = GetShaderResourceView(myDevice, myMaterialPath + aMaterialName + "/" + aMaterialName + "_c.dds");
		newTextures[1] = GetShaderResourceView(myDevice, myMaterialPath + aMaterialName + "/" + aMaterialName + "_m.dds");
		newTextures[2] = GetShaderResourceView(myDevice, myMaterialPath + aMaterialName + "/" + aMaterialName + "_n.dds");

		myMaterials.emplace(aMaterialName, newTextures);
	}

	std::array<ID3D11ShaderResourceView*, 3> textures;
	textures[0] = myMaterials[aMaterialName][0].Get();
	textures[1] = myMaterials[aMaterialName][1].Get();
	textures[2] = myMaterials[aMaterialName][2].Get();
	return textures;
}

void CMaterialHandler::ReleaseMaterial(const std::string aMaterialName)
{
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

ID3D11ShaderResourceView* CMaterialHandler::GetShaderResourceView(ID3D11Device* aDevice, std::string aTexturePath)
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
{
}

CMaterialHandler::~CMaterialHandler()
{
}
