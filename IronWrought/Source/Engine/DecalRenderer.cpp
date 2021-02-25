#include "stdafx.h"
#include "DecalRenderer.h"
#include "RenderManager.h"
#include "CameraComponent.h"
#include "DecalComponent.h"
#include "Decal.h"
#include "GraphicsHelpers.h"

#include "Engine.h"
#include "WindowHandler.h"

#include <fstream>

CDecalRenderer::CDecalRenderer()
    : myContext(nullptr)
    , myFrameBuffer(nullptr)
    , myObjectBuffer(nullptr)
    , myVertexBuffer(nullptr)
    , myIndexBuffer(nullptr)
	, myVertexShader(nullptr)
	, myPixelShader(nullptr)
    , mySamplerState(nullptr)
    , myInputLayout(nullptr)
    , myPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
    , myNumberOfVertices(0)
    , myNumberOfIndices(0)
    , myStride(0)
    , myOffset(0)
{
}

CDecalRenderer::~CDecalRenderer()
{
}

bool CDecalRenderer::Init(CDirectXFramework* aFramework)
{
	myContext = aFramework->GetContext();
	ENGINE_ERROR_BOOL_MESSAGE(myContext, "Context could not be set.");

	ID3D11Device* device = aFramework->GetDevice();
	ENGINE_ERROR_BOOL_MESSAGE(device, "Device is nullptr");

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(SFrameBufferData);
 	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer), "Frame Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SObjectBufferData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer), "Object Buffer could not be created.");

    //Vertex Setup
    struct Vertex
    {
        float x, y, z, w;
        float nx, ny, nz, nw;
        float tx, ty, tz, tw;
        float bx, by, bz, bw;
        float u, v;
    } vertices[24] = {
        // X      Y      Z      W,    nX, nY, nZ, nW,   tX, tY, tZ, tW,    bX, bY, bZ, bW,    UV	
        { -0.5f, -0.5f, -0.5f,  1,   -1,  0,  0,  0,    0,  0,  1,  0,     0,  1,  0,  0,     0, 0 },
        {  0.5f, -0.5f, -0.5f,  1,    1,  0,  0,  0,    0,  0, -1,  0,     0,  1,  0,  0,     1, 0 },
        { -0.5f,  0.5f, -0.5f,  1,   -1,  0,  0,  0,    0,  0,  1,  0,     0,  1,  0,  0,     0, 1 },
        {  0.5f,  0.5f, -0.5f,  1,    1,  0,  0,  0,    0,  0, -1,  0,     0,  1,  0,  0,     1, 1 },
        { -0.5f, -0.5f,  0.5f,  1,   -1,  0,  0,  0,    0,  0,  1,  0,     0,  1,  0,  0,     0, 0 },
        {  0.5f, -0.5f,  0.5f,  1,    1,  0,  0,  0,    0,  0, -1,  0,     0,  1,  0,  0,     1, 0 },
        { -0.5f,  0.5f,  0.5f,  1,   -1,  0,  0,  0,    0,  0,  1,  0,     0,  1,  0,  0,     0, 1 },
        {  0.5f,  0.5f,  0.5f,  1,    1,  0,  0,  0,    0,  0, -1,  0,     0,  1,  0,  0,     1, 1 },
        // X      Y      Z      W,    nX, nY, nZ, nW,   nX, nY, nZ, nW,    nX, nY, nZ, nW,    UV
        { -0.5f, -0.5f, -0.5f,  1,    0, -1,  0,  0,    1,  0,  0,  0,     0,  0,  1,  0,     0, 0 },
        { -0.5f,  0.5f, -0.5f,  1,    0,  0, -1,  0,   -1,  0,  0,  0,     0,  1,  0,  0,     1, 0 },
        { -0.5f, -0.5f,  0.5f,  1,    0, -1,  0,  0,    1,  0,  0,  0,     0,  0,  1,  0,     0, 1 },
        { -0.5f,  0.5f,  0.5f,  1,    0,  1,  0,  0,   -1,  0,  0,  0,     0,  0,  1,  0,     1, 1 },
        {  0.5f, -0.5f, -0.5f,  1,    0,  0, -1,  0,   -1,  0,  0,  0,     0,  1,  0,  0,     0, 0 },
        {  0.5f,  0.5f, -0.5f,  1,    0,  0, -1,  0,   -1,  0,  0,  0,     0,  1,  0,  0,     1, 0 },
        {  0.5f, -0.5f,  0.5f,  1,    0, -1,  0,  0,    1,  0,  0,  0,     0,  0,  1,  0,     0, 1 },
        {  0.5f,  0.5f,  0.5f,  1,    0,  1,  0,  0,   -1,  0,  0,  0,     0,  0,  1,  0,     1, 1 },
        // X      Y      Z      W,    nX, nY, nZ, nW,   nX, nY, nZ, nW,    nX, nY, nZ, nW,    UV
        { -0.5f, -0.5f, -0.5f,  1,    0,  0, -1,  0,   -1,  0,  0,  0,     0,  1,  0,  0,     0, 0 },
        {  0.5f, -0.5f, -0.5f,  1,    0, -1,  0,  0,    1,  0,  0,  0,     0,  0,  1,  0,     1, 0 },
        { -0.5f, -0.5f,  0.5f,  1,    0,  0,  1,  0,    1,  0,  0,  0,     0,  1,  0,  0,     0, 1 },
        {  0.5f, -0.5f,  0.5f,  1,    0,  0,  1,  0,    1,  0,  0,  0,     0,  1,  0,  0,     1, 1 },
        { -0.5f,  0.5f, -0.5f,  1,    0,  1,  0,  0,   -1,  0,  0,  0,     0,  0,  1,  0,     0, 0 },
        {  0.5f,  0.5f, -0.5f,  1,    0,  1,  0,  0,   -1,  0,  0,  0,     0,  0,  1,  0,     1, 0 },
        { -0.5f,  0.5f,  0.5f,  1,    0,  0,  1,  0,    1,  0,  0,  0,     0,  1,  0,  0,     0, 1 },
        {  0.5f,  0.5f,  0.5f,  1,    0,  0,  1,  0,    1,  0,  0,  0,     0,  1,  0,  0,     1, 1 }
    };
    //Index Setup
    unsigned int indices[36] = {
        0,2,1,
        2,3,1,
        4,5,7,
        4,7,6,
        8,10,9,
        10,11,9,
        12,13,15,
        12,15,14,
        16,17,18,
        18,17,19,
        20,23,21,
        20,22,23
    };

    D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
    vertexBufferDesc.ByteWidth = sizeof(vertices);
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subVertexResourceData = { 0 };
    subVertexResourceData.pSysMem = vertices;

    ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateBuffer(&vertexBufferDesc, &subVertexResourceData, &myVertexBuffer), "Vertex Buffer could not be created.");

    D3D11_BUFFER_DESC indexBufferDesc = { 0 };
    indexBufferDesc.ByteWidth = sizeof(indices);
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
    indexSubresourceData.pSysMem = indices;

    ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &myIndexBuffer), "Index Buffer could not be created.");

    myNumberOfVertices = static_cast<UINT>(sizeof(vertices) / sizeof(Vertex));
    myNumberOfIndices = static_cast<UINT>(sizeof(indices) / sizeof(UINT));
    myStride = sizeof(Vertex);
    myOffset = 0;

	std::string vsData;
	Graphics::CreateVertexShader("Shaders/DecalVertexShader.cso", aFramework, &myVertexShader, vsData);
	Graphics::CreatePixelShader("Shaders/DecalPixelShader.cso", aFramework, &myPixelShader);

    //Sampler
    D3D11_SAMPLER_DESC samplerDesc = { };
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = 10;

    ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateSamplerState(&samplerDesc, &mySamplerState), "Sampler State could not be created.");

    //Layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION" ,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL"   ,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT"  ,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BITANGENT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV"	    ,	0, DXGI_FORMAT_R32G32_FLOAT	     , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &myInputLayout), "Input Layout could not be created.");

    myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return true;
}

void CDecalRenderer::Render(CCameraComponent* aCamera, std::vector<CGameObject*>& aGameObjectList)
{
	if (aGameObjectList.empty())
		return;

	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	SM::Matrix projectionMatrix = aCamera->GetProjection();
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToProjectionSpace = projectionMatrix;
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToCameraFromProjection = projectionMatrix.Invert();
	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	myContext->IASetPrimitiveTopology(myPrimitiveTopology);
	myContext->IASetInputLayout(myInputLayout);
	myContext->VSSetShader(myVertexShader, nullptr, 0);
	myContext->PSSetShader(myPixelShader, nullptr, 0);
	myContext->PSSetSamplers(0, 1, &mySamplerState);
	myContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &myStride, &myOffset);
	myContext->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    for (auto& gameObject : aGameObjectList)
	{
		CDecalComponent* decalComponent = gameObject->GetComponent<CDecalComponent>();
		if (decalComponent == nullptr)
			continue;

		if (decalComponent->GetMyDecal() == nullptr)
			continue;

		CDecal* decal = decalComponent->GetMyDecal();
		CDecal::SDecalData decalData = decal->GetDecalData();

		myObjectBufferData.myToWorld = gameObject->myTransform->GetWorldMatrix();
		myObjectBufferData.myToObjectSpace = gameObject->myTransform->GetWorldMatrix().Invert();
		myObjectBufferData.myAlphaClipThreshold = decalData.myAlphaClipThreshold;
		BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetShaderResources(0, 3, &decalData.myMaterial[0]);
		
		myContext->DrawIndexed(myNumberOfIndices, 0, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}
}
