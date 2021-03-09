#include "stdafx.h"
#include "ModelFactory.h"
#include "Engine.h"
#include <d3d11.h>
#include <fstream>
#include "DDSTextureLoader.h"
#include "ModelMath.h"
#include "Model.h"
#include <UnityFactory.h>// <-- Why?
#include "MaterialHandler.h"


#ifdef _DEBUG
#pragma comment(lib, "ModelLoader_Debug.lib")
#endif
#ifdef NDEBUG
#pragma comment(lib, "ModelLoader_Release.lib")
#endif

//#define USING_FBX_MATERIALS

#define ALLOW_ANIMATIONS

CModelFactory* CModelFactory::ourInstance = nullptr;
CModelFactory* CModelFactory::GetInstance()
{
	return ourInstance;
}

CModelFactory::CModelFactory() 
	//: myEngine(nullptr)
	: myFramework(nullptr)
	, myOutlineModelSubset(nullptr)
{
	ourInstance = this;
}

CModelFactory::~CModelFactory()
{
	ourInstance = nullptr;
	myFramework = nullptr;

	auto itPBR = myModelMap.begin();
	while (itPBR != myModelMap.end())
	{
		delete itPBR->second;
		itPBR->second = nullptr;
		++itPBR;
	}
	
	auto itInstaned = myInstancedModelMap.begin();
	while (itInstaned != myInstancedModelMap.end())
	{
		delete itInstaned->second;
		itInstaned->second = nullptr;
		++itInstaned;
	}
}


bool CModelFactory::Init(CDirectXFramework* aFramework)
{
	myFramework = aFramework;
	return true;
}

CModel* CModelFactory::GetModel(std::string aFilePath)
{
	if (myModelMap.find(aFilePath) == myModelMap.end())
	{
		myModelMapReferences[aFilePath] = 1;
		return LoadModel(aFilePath);
	}
	
	myModelMapReferences[aFilePath] += 1;
	
	return myModelMap.at(aFilePath);
}

CModel* CModelFactory::LoadModel(std::string aFilePath)
{
	// Loading
	const size_t last_slash_idx = aFilePath.find_last_of("\\/");
	std::string modelDirectory = aFilePath.substr(0, last_slash_idx + 1);
	std::string modelName = aFilePath.substr(last_slash_idx + 1, aFilePath.size() - last_slash_idx - 5);

	CFBXLoaderCustom modelLoader;
	CLoaderModel* loaderModel = modelLoader.LoadModel(aFilePath.c_str());
	ENGINE_ERROR_BOOL_MESSAGE(loaderModel, aFilePath.append(" could not be loaded.").c_str());

	// Mesh Data
	size_t numberOfMeshes = loaderModel->myMeshes.size();
	std::vector<SMeshData> meshData;
	meshData.resize(numberOfMeshes);

	CLoaderMesh* mesh = loaderModel->myMeshes[0];
	myMeshesMap.emplace(aFilePath, mesh);


	unsigned int vertexSize = mesh->myVertexBufferSize;
	unsigned int vertexCount = mesh->myVertexCount;

	std::vector<Vector3> vertexPositions;
	vertexPositions.reserve(vertexCount);
	for (unsigned i = 0; i < vertexCount * vertexSize; i += vertexSize) {
		Vector3 vertexPosition = {};
		memcpy(&vertexPosition, &mesh->myVerticies[i], sizeof(Vector3));
		vertexPositions.emplace_back(vertexPosition);
	}
	myFBXVertexMap.emplace(aFilePath, vertexPositions);

	
	SMeshFilter meshFilter;
	meshFilter.myIndexes = mesh->myIndexes;
	meshFilter.myVertecies = vertexPositions;

	for (unsigned int i = 0; i < numberOfMeshes; ++i)
	{
		mesh = loaderModel->myMeshes[i];

		// Vertex Buffer
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = mesh->myVertexBufferSize * mesh->myVertexCount;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subVertexResourceData = { 0 };
		subVertexResourceData.pSysMem = mesh->myVerticies;

		if (vertexBufferDesc.ByteWidth == 0) {
			return nullptr;
		}
		ID3D11Buffer* vertexBuffer;
		ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateBuffer(&vertexBufferDesc, &subVertexResourceData, &vertexBuffer), "Vertex Buffer could not be created.");

		// Index Buffer
		D3D11_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<UINT>(mesh->myIndexes.size());
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subIndexResourceData = { 0 };
		subIndexResourceData.pSysMem = mesh->myIndexes.data();

		ID3D11Buffer* indexBuffer;
		ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateBuffer(&indexBufferDesc, &subIndexResourceData, &indexBuffer), "Index Buffer could not be created.");

		meshData[i].myNumberOfVertices = mesh->myVertexCount;
		meshData[i].myNumberOfIndices = static_cast<UINT>(mesh->myIndexes.size());
		meshData[i].myStride = mesh->myVertexBufferSize;
		meshData[i].myOffset = 0;
		meshData[i].myMaterialIndex = mesh->myModel->myMaterialIndices[i];
		meshData[i].myVertexBuffer = vertexBuffer;
		meshData[i].myIndexBuffer = indexBuffer;
	}

	//VertexShader
	std::ifstream vsFile;
#ifdef ALLOW_ANIMATIONS
	if (mesh->myModel->myNumBones > 0)
		vsFile.open("Shaders/AnimatedVertexShader.cso", std::ios::binary);
	else 
		vsFile.open("Shaders/VertexShader.cso", std::ios::binary);
#else
	vsFile.open("Shaders/VertexShader.cso", std::ios::binary);
#endif
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader), "Vertex Shader could not be created.");

	vsFile.close();

	//PixelShader
	std::ifstream psFile;
	psFile.open("Shaders/PBRPixelShader.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

	ID3D11PixelShader* pixelShader;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader), "Pixel Shader could not be created.");

	psFile.close();

	//Sampler
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDesc = { };
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 10;

	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateSamplerState(&samplerDesc, &sampler), "Sampler State could not be created.");

	//Layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEID", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"INSTANCETRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	ID3D11InputLayout* inputLayout;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &inputLayout), "Input Layout could not be created.");

	ID3D11Device* device = myFramework->GetDevice();
	std::string modelDirectoryAndName = modelDirectory + modelName;

	// Check for detail normal
	ID3D11ShaderResourceView* detailNormal1 = nullptr;
	ID3D11ShaderResourceView* detailNormal2 = nullptr;
	ID3D11ShaderResourceView* detailNormal3 = nullptr;
	ID3D11ShaderResourceView* detailNormal4 = nullptr;
	std::string dnsuffix = aFilePath.substr(aFilePath.length() - 7, 3);
	if (dnsuffix == "_dn")
	{
		detailNormal1 = GetShaderResourceView(device, "Assets/3D/Exempel_Modeller/DetailNormals/Tufted_Leather/dn_25cm_N.dds");
		detailNormal2 = GetShaderResourceView(device, "Assets/3D/Exempel_Modeller/DetailNormals/4DN/dns/dn_CarbonFibre_n.dds");
		detailNormal3 = GetShaderResourceView(device, "Assets/3D/Exempel_Modeller/DetailNormals/4DN/dns/dn_Wool_n.dds");
		detailNormal4 = GetShaderResourceView(device, "Assets/3D/Exempel_Modeller/DetailNormals/4DN/dns/dn_PlasticPolymer_n.dds");
	}

#ifdef USING_FBX_MATERIALS
	std::vector<std::array<ID3D11ShaderResourceView*, 3>> materials;
	std::vector<std::string> materialNames;
	for (unsigned int i = 0; i < loaderModel->myMaterials.size(); ++i) {
		std::string materialName = loaderModel->myMaterials[loaderModel->myMaterialIndices[i]];
		materials.push_back(CMainSingleton::MaterialHandler().RequestMaterial(materialName));
		materialNames.push_back(materialName);
	}
#else
	std::vector<std::string> materialNames;
	std::vector<std::array<ID3D11ShaderResourceView*, 3>> materials;
	for (unsigned int i = 0; i < loaderModel->myMaterials.size(); ++i) {
		ID3D11ShaderResourceView* diffuseResourceView = GetShaderResourceView(device, (modelDirectory + modelName/*modelDirectoryAndName*/ + "_c.dds"));
		ID3D11ShaderResourceView* materialResourceView = GetShaderResourceView(device, (modelDirectory + modelName/*modelDirectoryAndName*/ + "_m.dds"));
		ID3D11ShaderResourceView* normalResourceView = GetShaderResourceView(device, (modelDirectory + modelName/*modelDirectoryAndName*/ + "_n.dds"));
		materials.push_back({ diffuseResourceView, materialResourceView, normalResourceView });
		materialNames.push_back(modelName);
	}
	ID3D11ShaderResourceView* tintMapResourceView = GetShaderResourceView(device, (modelDirectory + modelName + "_t.dds"));
#endif

	delete loaderModel;

	//Model
	CModel* model = new CModel();
	ENGINE_ERROR_BOOL_MESSAGE(model, "Empty model could not be loaded.");

	CModel::SModelData modelData;
	modelData.myMeshes = meshData;
	modelData.myMeshFilter = meshFilter;
	modelData.myVertexShader = vertexShader;
	modelData.myPixelShader = pixelShader;
	modelData.mySamplerState = sampler;
	modelData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	modelData.myInputLayout = inputLayout;
	modelData.myMaterials = materials;
	modelData.myMaterialNames = materialNames;

	modelData.myDetailNormals[0] = detailNormal1;
	modelData.myDetailNormals[1] = detailNormal2;
	modelData.myDetailNormals[2] = detailNormal3;
	modelData.myDetailNormals[3] = detailNormal4;

	modelData.myTintMap = tintMapResourceView;

	model->Init(modelData);
#ifdef ALLOW_ANIMATIONS
	model->HasBones(mesh->myModel->myNumBones > 0);
#endif

	myModelMap.emplace(aFilePath, model);
	return model;
}

CModel* CModelFactory::GetOutlineModelSubset()
{
	if (myOutlineModelSubset) {
		return myOutlineModelSubset;
	}

	//Start Shader
	std::ifstream vsFile;
	vsFile.open("Shaders/AnimatedVertexShader.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader), "Vertex Shader could not be created.");
	vsFile.close();

	std::ifstream psFile;
	psFile.open("Shaders/OutlinePixelShader.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	ID3D11PixelShader* pixelShader;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader), "Pixel Shader could not be created.");
	psFile.close();
	//End Shader



	myOutlineModelSubset = new CModel();

	CModel::SModelData modelData;
	modelData.myVertexShader = vertexShader;
	modelData.myPixelShader = pixelShader;
	modelData.mySamplerState = nullptr;
	modelData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	modelData.myInputLayout = nullptr;
	myOutlineModelSubset->Init(modelData);

	return myOutlineModelSubset;
}

std::vector<DirectX::SimpleMath::Vector3>& CModelFactory::GetVertexPositions(const std::string& aFilePath)
{
	return myFBXVertexMap.at(aFilePath);
}

CLoaderMesh*& CModelFactory::GetMeshes(const std::string& aFilePath)
{
	return myMeshesMap.at(aFilePath);
}

void CModelFactory::ClearModel(std::string aFilePath, int aNumberOfInstances)
{
	if (myModelMap.find(aFilePath) != myModelMap.end())
	{
		myModelMapReferences[aFilePath] -= 1;
		if (myModelMapReferences[aFilePath] <= 0)
		{
			myModelMapReferences[aFilePath] = 0;
			myModelMap[aFilePath]->~CModel();
			myModelMap.erase(aFilePath);
		}
	}

	if (myInstancedModelMap.find({ aFilePath, aNumberOfInstances }) != myInstancedModelMap.end())
	{
		myInstancedModelMapReferences[aFilePath] -= 1;
		if (myInstancedModelMapReferences[aFilePath] <= 0)
		{
			myInstancedModelMapReferences[aFilePath] = 0;
			myInstancedModelMap[{ aFilePath, aNumberOfInstances }]->~CModel();
			myInstancedModelMap.erase({ aFilePath, aNumberOfInstances });
		}
	}
}

CModel* CModelFactory::GetInstancedModel(std::string aFilePath, int aNumberOfInstanced)
{
	SInstancedModel instancedModel = {aFilePath, aNumberOfInstanced};
	if (myInstancedModelMap.find(instancedModel) == myInstancedModelMap.end())
	{
		myInstancedModelMapReferences[aFilePath] = 1;
		return CreateInstancedModels(aFilePath, aNumberOfInstanced);
	}

	myInstancedModelMapReferences[aFilePath] += 1;

	return myInstancedModelMap[instancedModel];
}

void CModelFactory::ClearFactory()
{
	auto itPBR = myModelMap.begin();
	while (itPBR != myModelMap.end())
	{
		//delete itPBR->second;
		itPBR->second = nullptr;
		++itPBR;
	}
	myModelMap.clear();

	auto itInstaned = myInstancedModelMap.begin();
	while (itInstaned != myInstancedModelMap.end())
	{
		delete itInstaned->second;
		itInstaned->second = nullptr;
		++itInstaned;
	}
	myInstancedModelMap.clear();
}

CModel* CModelFactory::CreateInstancedModels(std::string aFilePath, int aNumberOfInstanced)
{
	const size_t last_slash_idx = aFilePath.find_last_of("\\/");
	std::string modelDirectory = aFilePath.substr(0, last_slash_idx + 1);
	std::string modelName = aFilePath.substr(last_slash_idx + 1, aFilePath.size() - last_slash_idx - 5);

	CFBXLoaderCustom modelLoader;
	CLoaderModel* loaderModel = modelLoader.LoadModel(aFilePath.c_str());
	ENGINE_ERROR_BOOL_MESSAGE(loaderModel, aFilePath.append(" could not be loaded.").c_str());

	// Mesh Data
	size_t numberOfMeshes = loaderModel->myMeshes.size();
	std::vector<SInstancedMeshData> meshData;
	meshData.resize(numberOfMeshes);

	CLoaderMesh* mesh = loaderModel->myMeshes[0];

	unsigned int vertexSize = mesh->myVertexBufferSize;
	unsigned int vertexCount = mesh->myVertexCount;

	std::vector<Vector3> vertexPositions;
	vertexPositions.reserve(vertexCount);
	for (unsigned i = 0; i < vertexCount * vertexSize; i += vertexSize) {
		Vector3 vertexPosition = {};
		memcpy(&vertexPosition, &mesh->myVerticies[i], sizeof(Vector3));
		vertexPositions.emplace_back(vertexPosition);
	}


	SMeshFilter meshFilter;
	meshFilter.myIndexes = mesh->myIndexes;
	meshFilter.myVertecies = vertexPositions;

	for (unsigned int i = 0; i < numberOfMeshes; ++i)
	{
		mesh = loaderModel->myMeshes[i];

		// Vertex Buffer
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = mesh->myVertexBufferSize * mesh->myVertexCount;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subVertexResourceData = { 0 };
		subVertexResourceData.pSysMem = mesh->myVerticies;

		if (vertexBufferDesc.ByteWidth == 0) {
			return nullptr;
		}
		ID3D11Buffer* vertexBuffer;
		ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateBuffer(&vertexBufferDesc, &subVertexResourceData, &vertexBuffer), "Vertex Buffer could not be created.");

		// Index Buffer
		D3D11_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<UINT>(mesh->myIndexes.size());
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subIndexResourceData = { 0 };
		subIndexResourceData.pSysMem = mesh->myIndexes.data();

		ID3D11Buffer* indexBuffer;
		ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateBuffer(&indexBufferDesc, &subIndexResourceData, &indexBuffer), "Index Buffer could not be created.");

		meshData[i].myNumberOfVertices = mesh->myVertexCount;
		meshData[i].myNumberOfIndices = static_cast<UINT>(mesh->myIndexes.size());
		meshData[i].myStride[0] = mesh->myVertexBufferSize;
		meshData[i].myStride[1] = sizeof(CModel::SInstanceType);
		meshData[i].myOffset[0] = 0;	
		meshData[i].myOffset[1] = 0;	
		meshData[i].myMaterialIndex = mesh->myModel->myMaterialIndices[i];
		meshData[i].myVertexBuffer = vertexBuffer;
		meshData[i].myIndexBuffer = indexBuffer;
	}

	//Model
	CModel* model = new CModel();
	ENGINE_ERROR_BOOL_MESSAGE(model, "Empty model could not be loaded.");
	model->InstanceCount(aNumberOfInstanced);

	//Instance Buffer
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(CModel::SInstanceType) * model->InstanceCount();
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	ID3D11Buffer* instanceBuffer;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateBuffer(&instanceBufferDesc, nullptr, &instanceBuffer), "Instance Buffer could not be created.");

	//VertexShader
	std::ifstream vsFile;
#ifdef ALLOW_ANIMATIONS
	if (mesh->myModel->myNumBones > 0)
	{
		vsFile.open("Shaders/AnimatedVertexShader.cso", std::ios::binary);
	}
	else {
		vsFile.open("Shaders/InstancedVertexShader.cso", std::ios::binary);
	}
#else
	vsFile.open("Shaders/InstancedVertexShader.cso", std::ios::binary);
#endif

	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader), "Vertex Shader could not be created.");

	vsFile.close();


	//PixelShader
	std::ifstream psFile;
	psFile.open("Shaders/PBRPixelShader.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

	ID3D11PixelShader* pixelShader;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader), "Pixel Shader could not be created.");

	psFile.close();

	//Sampler
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDesc = { };
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 10;

	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateSamplerState(&samplerDesc, &sampler), "Sampler State could not be created.");

	//Layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION"			,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL"			,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT"			,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BITANGENT"		,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV"				,	0, DXGI_FORMAT_R32G32_FLOAT		 , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEID"			,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEWEIGHT"		,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"INSTANCETRANSFORM",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM",	1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM",	2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCETRANSFORM",	3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	ID3D11InputLayout* inputLayout;
	ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &inputLayout), "Input Layout could not be created.");

	ID3D11Device* device = myFramework->GetDevice();
	std::string modelDirectoryAndName = modelDirectory + modelName;

#ifdef USING_FBX_MATERIALS
	std::vector<std::array<ID3D11ShaderResourceView*, 3>> materials;
	std::vector<std::string> materialNames;
	for (unsigned int i = 0; i < loaderModel->myMaterials.size(); ++i) {
		std::string materialName = loaderModel->myMaterials[loaderModel->myMaterialIndices[i]];
		materials.push_back(CMainSingleton::MaterialHandler().RequestMaterial(materialName));
		materialNames.push_back(materialName);
	}
#else
	std::vector<std::array<ID3D11ShaderResourceView*, 3>> materials;
	std::vector<std::string> materialNames;
	
	for (unsigned int i = 0; i < loaderModel->myMaterials.size(); ++i) {
		ID3D11ShaderResourceView* diffuseResourceView = GetShaderResourceView(device, (modelDirectory + modelName/*modelDirectoryAndName*/ + "_c.dds"));
		ID3D11ShaderResourceView* materialResourceView = GetShaderResourceView(device, (modelDirectory + modelName/*modelDirectoryAndName*/ + "_m.dds"));
		ID3D11ShaderResourceView* normalResourceView = GetShaderResourceView(device, (modelDirectory + modelName/*modelDirectoryAndName*/ + "_n.dds"));
		materials.push_back({ diffuseResourceView, materialResourceView, normalResourceView });
		materialNames.push_back(modelName);
	}

#endif

	// Check for detail normal
	ID3D11ShaderResourceView* detailNormal1 = nullptr;
	ID3D11ShaderResourceView* detailNormal2 = nullptr;
	ID3D11ShaderResourceView* detailNormal3 = nullptr;
	ID3D11ShaderResourceView* detailNormal4 = nullptr;
	std::string dnsuffix = aFilePath.substr(aFilePath.length() - 7, 3);
	if (dnsuffix == "_dn")
	{
		detailNormal1 = GetShaderResourceView(device, "Assets/3D/Exempel_Modeller/DetailNormals/Tufted_Leather/dn_25cm_N.dds");
		detailNormal2 = GetShaderResourceView(device, "Assets/3D/Exempel_Modeller/DetailNormals/4DN/dns/dn_CarbonFibre_n.dds");
		detailNormal3 = GetShaderResourceView(device, "Assets/3D/Exempel_Modeller/DetailNormals/4DN/dns/dn_Wool_n.dds");
		detailNormal4 = GetShaderResourceView(device, "Assets/3D/Exempel_Modeller/DetailNormals/4DN/dns/dn_PlasticPolymer_n.dds");
	}

	CModel::SModelInstanceData modelInstanceData;
	modelInstanceData.myMeshes = meshData;
	modelInstanceData.myMeshFilter = meshFilter;
	modelInstanceData.myInstanceBuffer = instanceBuffer;
	modelInstanceData.myVertexShader = vertexShader;
	modelInstanceData.myPixelShader = pixelShader;
	modelInstanceData.mySamplerState = sampler;
	modelInstanceData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	modelInstanceData.myInputLayout = inputLayout;
	modelInstanceData.myMaterials = materials;
	modelInstanceData.myMaterialNames = materialNames;

	modelInstanceData.myDetailNormals[0] = detailNormal1;
	modelInstanceData.myDetailNormals[1] = detailNormal2;
	modelInstanceData.myDetailNormals[2] = detailNormal3;
	modelInstanceData.myDetailNormals[3] = detailNormal4;

	model->Init(modelInstanceData);
	SInstancedModel instancedModel = { aFilePath, aNumberOfInstanced };

	myInstancedModelMap[instancedModel] = model;
	return model;
}

ID3D11ShaderResourceView* CModelFactory::GetShaderResourceView(ID3D11Device* aDevice, std::string aTexturePath)
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
		errorTexturePath = "Assets/ErrorTextures/Checkboard_128x128" + errorTexturePath;

		wchar_t* wideErrorPath = new wchar_t[errorTexturePath.length() + 1];
		std::copy(errorTexturePath.begin(), errorTexturePath.end(), wideErrorPath);
		wideErrorPath[errorTexturePath.length()] = 0;

		DirectX::CreateDDSTextureFromFile(aDevice, wideErrorPath, nullptr, &shaderResourceView);
		delete[] wideErrorPath;
	}

	delete[] widePath;
	return shaderResourceView;
}