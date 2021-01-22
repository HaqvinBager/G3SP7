#pragma once
#include <array>
#include <map>

class CDirectXFramework;

class CMaterialHandler
{
	friend class CMainSingleton;
	friend class CEngine;

public:
	std::array<ID3D11ShaderResourceView*, 3> RequestMaterial(const std::string aMaterialName);
	void ReleaseMaterial(const std::string aMaterialName);

protected:
	bool Init(CDirectXFramework* aFramwork);

private:
	ID3D11ShaderResourceView* GetShaderResourceView(ID3D11Device* aDevice, std::string aTexturePath);

private:
	std::map<std::string, std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3>> myMaterials;
	ID3D11Device* myDevice;
	const std::string myMaterialPath = ASSETPATH + "Assets/Graphics/Textures/Materials/";

private:
	CMaterialHandler();
	~CMaterialHandler();
};

