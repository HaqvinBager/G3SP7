#pragma once
#include <array>
#include <map>

class CDirectXFramework;

typedef std::array<ID3D11ShaderResourceView*, 3> material;

struct SVertexPaintData
{
	std::vector<std::string> myRGBMaterialNames;
	unsigned int myVertexColorID;
};

class CMaterialHandler
{
	friend class CMainSingleton;
	friend class CEngine;

public:
	std::array<ID3D11ShaderResourceView*, 3> RequestMaterial(const std::string& aMaterialName);
	std::array<ID3D11ShaderResourceView*, 9> GetVertexPaintMaterials(const std::vector<std::string>& someMaterialNames);
	void ReleaseMaterial(const std::string& aMaterialName);

	SVertexPaintData RequestVertexColorID(int aGameObjectID);
	std::vector<DirectX::SimpleMath::Vector3>& GetVertexColors(unsigned int aVertexColorID);
	ID3D11Buffer* GetVertexColorBuffer(unsigned int aVertexColorID);
	void ReleaseVertexColors(unsigned int aVertexColorID);

protected:
	bool Init(CDirectXFramework* aFramwork);

private:
	ID3D11ShaderResourceView* GetShaderResourceView(ID3D11Device* aDevice, const std::string& aTexturePath);

private:
	std::map<std::string, std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3>> myMaterials;
	std::map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>> myVertexColors;
	std::map<unsigned int, ID3D11Buffer*> myVertexColorBuffers;
	std::map<std::string, int> myMaterialReferences;
	std::map<unsigned int, int> myVertexColorReferences;

	ID3D11Device* myDevice;
	const std::string myMaterialPath = ASSETPATH + "Assets/Graphics/Textures/Materials/";
	const std::string myVertexLinksPath = ASSETPATH + "Assets/Generated/PolybrushLinks_-10882.json";

private:
	CMaterialHandler();
	~CMaterialHandler();
};

