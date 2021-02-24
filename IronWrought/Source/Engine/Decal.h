#pragma once

struct SMeshData {
	ID3D11Buffer* myVertexBuffer = nullptr;
	ID3D11Buffer* myIndexBuffer = nullptr;
	UINT myNumberOfVertices = 0;
	UINT myNumberOfIndices = 0;
	UINT myStride = 0;
	UINT myOffset = 0;
	UINT myMaterialIndex = 0;
};

class CDecal
{
public:
	struct SDecalData {
		SMeshData myMesh;
		ID3D11VertexShader* myVertexShader = nullptr;
		ID3D11PixelShader* myPixelShader = nullptr;
		ID3D11SamplerState* mySamplerState = nullptr;
		D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology;
		ID3D11InputLayout* myInputLayout = nullptr;
		std::vector<std::array<ID3D11ShaderResourceView*, 3>> myMaterials{ };
		std::vector<std::string> myMaterialNames;
	};

public:
	CDecal();
	~CDecal();

	void Init(SDecalData data);
	SDecalData& GetDecalData();

private:
	SDecalData myDecalData;
};

