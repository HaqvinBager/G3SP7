#pragma once
#include <array>
#include <d3d11.h>

/* REMINDER (2021 02 09)
* Move SModelInstancedData to its own class: CInstancedModel!
*/

struct SMeshData {
	ID3D11Buffer* myVertexBuffer = nullptr;
	ID3D11Buffer* myIndexBuffer = nullptr;
	UINT myNumberOfVertices = 0;
	UINT myNumberOfIndices = 0;
	UINT myStride = 0;
	UINT myOffset = 0;
	UINT myMaterialIndex = 0;
};

struct SInstancedMeshData {
	ID3D11Buffer* myVertexBuffer = nullptr;
	ID3D11Buffer* myIndexBuffer = nullptr;
	UINT myNumberOfVertices = 0;
	UINT myNumberOfIndices = 0;
	UINT myStride[2] = { 0, 0 };
	UINT myOffset[2] = { 0, 0 };
	UINT myMaterialIndex = 0;
};

struct SMeshFilter {
	std::vector<unsigned int> myIndexes;
	std::vector<Vector3> myVertecies;
};

class CModel {
public:
	struct SModelData {
		std::vector<SMeshData> myMeshes { };
		SMeshFilter myMeshFilter;
		ID3D11VertexShader* myVertexShader = nullptr;
		ID3D11PixelShader* myPixelShader = nullptr;
		ID3D11SamplerState* mySamplerState = nullptr;
		D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology;
		ID3D11InputLayout* myInputLayout = nullptr;
		std::vector<std::array<ID3D11ShaderResourceView*, 3>> myMaterials { };
		std::vector<std::string> myMaterialNames;
		std::array<ID3D11ShaderResourceView*, 4> myDetailNormals { nullptr, nullptr, nullptr, nullptr };
	};

	struct SModelInstanceData {
		std::vector<SInstancedMeshData> myMeshes{ };
		SMeshFilter myMeshFilter;
		ID3D11Buffer* myInstanceBuffer = nullptr;
		ID3D11VertexShader* myVertexShader = nullptr;
		ID3D11PixelShader* myPixelShader = nullptr;
		ID3D11SamplerState* mySamplerState = nullptr;
		D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology;
		ID3D11InputLayout* myInputLayout = nullptr;
		std::vector<std::array<ID3D11ShaderResourceView*, 3>> myMaterials{ };
		std::vector<std::string> myMaterialNames;
		std::array<ID3D11ShaderResourceView*, 4> myDetailNormals { nullptr, nullptr, nullptr, nullptr };

		unsigned short myModelModifiers;
	};

	struct SInstanceType {
		DirectX::SimpleMath::Matrix myTransform;
	};

public:
	CModel();
	~CModel();

	void Init(SModelData data);
	SModelData& GetModelData();

public:
	void HasBones(const bool aHasBones) { myHasBones = aHasBones; }
	const bool HasBones() const { return myHasBones; }

public:
	void Init(SModelInstanceData aData);
	SModelInstanceData& GetModelInstanceData() { return myModelInstanceData; }

	void InstanceCount(int aCount);
	const int& InstanceCount() const { return myInstanceCount; }

	/*bool AddInstancedTransform(DirectX::SimpleMath::Matrix aTransform);
	std::vector<DirectX::SimpleMath::Matrix>& GetInstancedTransform() { return myTransforms; }*/

	const bool ShouldRenderWithAlpha() const;
	const unsigned int DetailNormalCount() const;
	const unsigned int UseTrimsheetNumber() const;

	void EvaluateModelSuffix(const std::string& aModelSuffix);

private:
	SModelData myModelData;
	bool myHasBones;

private:
	SModelInstanceData myModelInstanceData;
	int myInstanceCount;
};