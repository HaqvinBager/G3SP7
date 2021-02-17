#include "stdafx.h"
#include "DecalFactory.h"
#include "Engine.h"
#include <d3d11.h>
#include "FBXLoaderCustom.h"
#include "Decal.h"
#include "MaterialHandler.h"

#include <fstream>

#ifdef _DEBUG
#pragma comment(lib, "ModelLoader_Debug.lib")
#endif
#ifdef NDEBUG
#pragma comment(lib, "ModelLoader_Release.lib")
#endif

CDecalFactory* CDecalFactory::ourInstance = nullptr;

CDecalFactory* CDecalFactory::GetInstance()
{
    return ourInstance;
}

bool CDecalFactory::Init(CDirectXFramework* aFramework)
{
    myFramework = aFramework;
    return true;
}

void CDecalFactory::ClearFactory()
{
    auto itDecal = myDecalMap.begin();
    while (itDecal != myDecalMap.end())
    {
        itDecal->second = nullptr;
        ++itDecal;
    }
    myDecalMap.clear();
}

CDecal* CDecalFactory::GetDecal(std::string aFilePath)
{
    if (myDecalMap.find(aFilePath) == myDecalMap.end())
    {
        myDecalMapReferences[aFilePath] = 1;
        return LoadDecal(aFilePath);
    }

    myDecalMapReferences[aFilePath] += 1;

    return myDecalMap.at(aFilePath);
}

void CDecalFactory::ClearDecal(std::string aFilePath)
{
    if (myDecalMapReferences.find(aFilePath) != myDecalMapReferences.end())
    {
        myDecalMapReferences[aFilePath] -= 1;
        if (myDecalMapReferences[aFilePath] <= 0)
        {
            myDecalMapReferences[aFilePath] = 0;
            myDecalMap[aFilePath]->~CDecal();
            myDecalMap.erase(aFilePath);
        }
    }
}

CDecal* CDecalFactory::LoadDecal(std::string aFilePath)
{
    // Loading
    const size_t lastSlashIndex = aFilePath.find_last_of("\\/");
    std::string modelDirectory = aFilePath.substr(0, lastSlashIndex + 1);
    std::string modelName = aFilePath.substr(lastSlashIndex + 1, aFilePath.size() - lastSlashIndex - 5);

    CFBXLoaderCustom modelLoader;
    CLoaderModel* loaderModel = modelLoader.LoadModel(aFilePath.c_str());
    ENGINE_ERROR_BOOL_MESSAGE(loaderModel, aFilePath.append(" could not be loaded.").c_str());

    // Mesh Data
    SMeshData meshData;

    CLoaderMesh* mesh = loaderModel->myMeshes[0];

    unsigned int vertexSize = mesh->myVertexBufferSize;
    unsigned int vertexCount = mesh->myVertexCount;

    mesh = loaderModel->myMeshes[0];

    // Vertex Buffer
    D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
    vertexBufferDesc.ByteWidth = vertexSize * vertexCount;
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

    meshData.myNumberOfVertices = mesh->myVertexCount;
    meshData.myNumberOfIndices = static_cast<UINT>(mesh->myIndexes.size());
    meshData.myStride = mesh->myVertexBufferSize;
    meshData.myOffset = 0;
    meshData.myMaterialIndex = mesh->myModel->myMaterialIndices[0];
    meshData.myVertexBuffer = vertexBuffer;
    meshData.myIndexBuffer = indexBuffer;
    

    CDecal* decal = new CDecal();

    ID3D11VertexShader* vertexShader;
    std::string vertexShaderPath = "";
    std::string vsData = "";
    CreateVertexShader(vertexShaderPath, myFramework, &vertexShader, vsData);

    ID3D11PixelShader* pixelShader;
    std::string pixelShaderPath = "";
    CreatePixelShader(pixelShaderPath, myFramework, &pixelShader);

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
        {"UV"				,	0, DXGI_FORMAT_R32G32_FLOAT		 , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ID3D11InputLayout* inputLayout;
    ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &inputLayout), "Input Layout could not be created.");

    std::vector<std::array<ID3D11ShaderResourceView*, 3>> materials;
    std::vector<std::string> materialNames;
    for (unsigned int i = 0; i < loaderModel->myMaterials.size(); ++i) {
        std::string materialName = loaderModel->myMaterials[loaderModel->myMaterialIndices[i]];
        materials.push_back(CMainSingleton::MaterialHandler().RequestMaterial(materialName));
        materialNames.push_back(materialName);
    }

    CDecal::SDecalData decalData;
    decalData.myMesh = meshData;
    decalData.myVertexShader = vertexShader;
    decalData.myPixelShader = pixelShader;
    decalData.mySamplerState = sampler;
    decalData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    decalData.myInputLayout = inputLayout;
    decalData.myMaterials = materials;
    decalData.myMaterialNames = materialNames;

    decal->Init(decalData);

    myDecalMap.emplace(aFilePath, decal);
    return decal;
}

bool CDecalFactory::CreateVertexShader(std::string aFilepath, CDirectXFramework* aFramework, ID3D11VertexShader** outVertexShader, std::string& outShaderData)
{
    std::ifstream vsFile;
    vsFile.open(aFilepath, std::ios::binary);
    std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
    outShaderData = vsData;
    ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, outVertexShader), "Vertex Shader could not be created.");
    vsFile.close();
    return true;
}

bool CDecalFactory::CreatePixelShader(std::string aFilepath, CDirectXFramework* aFramework, ID3D11PixelShader** outPixelShader)
{
    std::ifstream psFile;
    psFile.open(aFilepath, std::ios::binary);
    std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
    ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, outPixelShader), "Pixel Shader could not be created.");
    psFile.close();
    return true;
}

CDecalFactory::CDecalFactory()
    : myFramework(nullptr)
{
    ourInstance = this;
}

CDecalFactory::~CDecalFactory()
{
    ourInstance = nullptr;
    myFramework = nullptr;
}
