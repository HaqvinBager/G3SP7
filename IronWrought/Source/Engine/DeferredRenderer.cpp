#include "stdafx.h"
#include "DeferredRenderer.h"
#include "DirectXFramework.h"
#include "RenderManager.h"
#include "GraphicsHelpers.h"

#include "Model.h"
#include "Camera.h"
#include "EnvironmentLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "GameObject.h"
#include "CameraComponent.h"
#include "ModelComponent.h"
#include "AnimationComponent.h"
#include "InstancedModelComponent.h"
#include "MaterialHandler.h"

#include <fstream>

namespace SM = DirectX::SimpleMath;

CDeferredRenderer::CDeferredRenderer()
	: myContext(nullptr)
	, myFrameBuffer(nullptr)
	, myObjectBuffer(nullptr)
	, myLightBuffer(nullptr)
	, myPointLightBuffer(nullptr)
	, mySpotLightBuffer(nullptr)
	, mySkyboxTransformBuffer(nullptr)
	, myPointLightVertexBuffer(nullptr)
	, myPointLightIndexBuffer(nullptr)
	, mySpotLightVertexBuffer(nullptr)
	, myFullscreenShader(nullptr)
	, myModelVertexShader(nullptr)
	, myInstancedModelVertexShader(nullptr)
	, myAnimationVertexShader(nullptr)
	, myVertexPaintModelVertexShader(nullptr)
	, myPointLightVertexShader(nullptr)
	, mySpotLightVertexShader(nullptr)
	, mySkyboxVertexShader(nullptr)
	, myPointLightGeometryShader(nullptr)
	, mySpotLightGeometryShader(nullptr)
	, myEnvironmentLightShader(nullptr)
	, myGBufferPixelShader(nullptr)
	, myPointLightShader(nullptr)
	, mySpotLightShader(nullptr)
	, myVertexPaintPixelShader(nullptr)
	, myDirectionalVolumetricLightShader(nullptr)
	, mySkyboxPixelShader(nullptr)
	, mySamplerState(nullptr)
	, myShadowSampler(nullptr)
	, myCurrentGBufferPixelShader(nullptr)
	, myRenderPassGBuffer(nullptr)
	, myCurrentRenderPassShader(nullptr)
	, myVertexPaintInputLayout(nullptr)
	, myPointLightInputLayout(nullptr)
	, myRenderPassIndex(9)
	, myBoneBuffer(nullptr)
	, myBoneBufferData()
	, myPointLightNumberOfVertices(0)
	, myPointLightNumberOfIndices(0)
	, myPointLightStride(0)
	, myPointLightOffset(0)
	, mySkyboxVertexBuffer(nullptr)
	, mySkyboxIndexBuffer(nullptr)
	, mySkyboxInputLayout(nullptr)
	, mySkyboxNumberOfVertices(0)
	, mySkyboxNumberOfIndices(0)
	, mySkyboxStride(0)
	, mySkyboxOffset(0)

{}

CDeferredRenderer::~CDeferredRenderer()
{
}

bool CDeferredRenderer::Init(CDirectXFramework* aFramework)
{
	myContext = aFramework->GetContext();
	ENGINE_ERROR_BOOL_MESSAGE(myContext, "Context could not be acquired from Framework.");

	ID3D11Device* device = aFramework->GetDevice();
	ENGINE_ERROR_BOOL_MESSAGE(device, "Device could not be acquired from Framework.");

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(SFrameBufferData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer), "Frame Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SObjectBufferData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer), "Object Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SLightBufferData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myLightBuffer), "Light Buffer could not be created.");
	
	bufferDescription.ByteWidth = sizeof(SPointLightBufferData);
	ENGINE_HR_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myPointLightBuffer), "Point Light Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SSpotLightBufferData);
	ENGINE_HR_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &mySpotLightBuffer), "Spot Light Buffer could not be created.");

	bufferDescription.ByteWidth = static_cast<UINT>(sizeof(SBoneBufferData) + (16 - (sizeof(SBoneBufferData) % 16)));
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myBoneBuffer), "Bone Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SSkyboxTransformData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &mySkyboxTransformBuffer), "Skybox Transform Buffer could not be created.");

	std::string vsData;
	Graphics::CreateVertexShader("Shaders/DeferredVertexShader.cso", aFramework, &myFullscreenShader, vsData);
	Graphics::CreateVertexShader("Shaders/DeferredModelVertexShader.cso", aFramework, &myModelVertexShader, vsData);
	Graphics::CreateVertexShader("Shaders/DeferredAnimationVertexShader.cso", aFramework, &myAnimationVertexShader, vsData);
	Graphics::CreateVertexShader("Shaders/DeferredInstancedModelVertexShader.cso", aFramework, &myInstancedModelVertexShader, vsData);

	Graphics::CreatePixelShader("Shaders/GBufferPixelShader.cso", aFramework, &myGBufferPixelShader);
	Graphics::CreatePixelShader("Shaders/DeferredLightEnvironmentShader.cso", aFramework, &myEnvironmentLightShader);
	Graphics::CreatePixelShader("Shaders/DeferredLightPointShader.cso", aFramework, &myPointLightShader);
	Graphics::CreatePixelShader("Shaders/DeferredLightSpotShader.cso", aFramework, &mySpotLightShader);
	Graphics::CreatePixelShader("Shaders/DirectionalVolumetricLightPixelShader.cso", aFramework, &myDirectionalVolumetricLightShader);

	LoadRenderPassPixelShaders(aFramework->GetDevice());

	// Vertex Paint 
	Graphics::CreatePixelShader("Shaders/DeferredVertexPaintPixelShader.cso", aFramework, &myVertexPaintPixelShader);
	Graphics::CreateVertexShader("Shaders/DeferredVertexPaintVertexShader.cso", aFramework, &myVertexPaintModelVertexShader, vsData);
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION"	,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL"	,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT"	,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BINORMAL"	,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV"		,	0, DXGI_FORMAT_R32G32_FLOAT		 , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR"	,	0, DXGI_FORMAT_R32G32B32_FLOAT	 , 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ENGINE_HR_MESSAGE(aFramework->GetDevice()->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &myVertexPaintInputLayout), "Vertex Paint Input Layout could not be created.");

	// Samplers
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 10;
	ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateSamplerState(&samplerDesc, &mySamplerState), "Sampler could not be created.");

	samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;

	ENGINE_HR_BOOL_MESSAGE(device->CreateSamplerState(&samplerDesc, &myShadowSampler), "Shadow Sampler could not be created.");

#pragma region Skybox
	// Skybox
	struct SkyboxVertex
	{
		float x, y, z;
	} vertices[24] = {
		// X      Y      Z    
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		// X      Y      Z    
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		// X      Y      Z    
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f }
	};
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

	D3D11_BUFFER_DESC skyboxVertexBufferDesc = { 0 };
	skyboxVertexBufferDesc.ByteWidth = sizeof(vertices);
	skyboxVertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	skyboxVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA skyboxSubVertexResourceData = { 0 };
	skyboxSubVertexResourceData.pSysMem = vertices;

	ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateBuffer(&skyboxVertexBufferDesc, &skyboxSubVertexResourceData, &mySkyboxVertexBuffer), "Skybox Vertex Buffer could not be created.");

	D3D11_BUFFER_DESC skyboxIndexBufferDesc = { 0 };
	skyboxIndexBufferDesc.ByteWidth = sizeof(indices);
	skyboxIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	skyboxIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA skyboxIndexSubresourceData = { 0 };
	skyboxIndexSubresourceData.pSysMem = indices;

	ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateBuffer(&skyboxIndexBufferDesc, &skyboxIndexSubresourceData, &mySkyboxIndexBuffer), "Skybox Index Buffer could not be created.");

	mySkyboxNumberOfVertices = static_cast<UINT>(sizeof(vertices) / sizeof(SkyboxVertex));
	mySkyboxNumberOfIndices = static_cast<UINT>(sizeof(indices) / sizeof(UINT));
	mySkyboxStride = sizeof(SkyboxVertex);
	mySkyboxOffset = 0;

	Graphics::CreateVertexShader("Shaders/SkyboxVertexShader.cso", aFramework, &mySkyboxVertexShader, vsData);
	Graphics::CreatePixelShader("Shaders/SkyboxPixelShader.cso", aFramework, &mySkyboxPixelShader);

	D3D11_INPUT_ELEMENT_DESC skyboxLayout[] =
	{
		{"POSITION"	,	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	ENGINE_HR_MESSAGE(aFramework->GetDevice()->CreateInputLayout(skyboxLayout, sizeof(skyboxLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &mySkyboxInputLayout), "Skybox Input Layout could not be created.");

	// Skybox
#pragma endregion

	return true;
}

void CDeferredRenderer::GenerateGBuffer(CCameraComponent* aCamera, std::vector<CGameObject*>& aGameObjectList, std::vector<CGameObject*>& aInstancedGameObjectList)
{
	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();

	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	// Toggling render passes
	if (myCurrentRenderPassShader == nullptr)
		myCurrentGBufferPixelShader = myGBufferPixelShader;
	else
		myCurrentGBufferPixelShader = myRenderPassGBuffer;

	for (auto& gameObject : aGameObjectList)
	{
		CModelComponent* modelComponent = gameObject->GetComponent<CModelComponent>();
		if (modelComponent == nullptr)
			continue;

		if (modelComponent->GetMyModel() == nullptr)
			continue;

		CModel* model = modelComponent->GetMyModel();
		const CModel::SModelData& modelData = model->GetModelData();

		myObjectBufferData.myToWorld = gameObject->myTransform->Transform();
		int dnCounter = 0;
		for (auto detailNormal : model->GetModelData().myDetailNormals)
		{
			if (detailNormal)
				++dnCounter;
		}
		myObjectBufferData.myNumberOfDetailNormals = dnCounter;

		BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");
		
		if (gameObject->GetComponent<CAnimationComponent>() != nullptr) {
			memcpy(myBoneBufferData.myBones, gameObject->GetComponent<CAnimationComponent>()->GetBones().data(), sizeof(Matrix) * 64);
		
			BindBuffer(myBoneBuffer, myBoneBufferData, "Bone Buffer");
		
			myContext->VSSetConstantBuffers(4, 1, &myBoneBuffer);
			myContext->VSSetShader(myAnimationVertexShader, nullptr, 0);
		}
		else
		{
			myContext->VSSetShader(myModelVertexShader, nullptr, 0);
		}
		myContext->IASetPrimitiveTopology(modelData.myPrimitiveTopology);
		myContext->IASetInputLayout(modelData.myInputLayout);

		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);

		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetShaderResources(8, 4, &modelData.myDetailNormals[0]);

		myContext->PSSetShader(myCurrentGBufferPixelShader, nullptr, 0);
		myContext->PSSetSamplers(0, 1, &modelData.mySamplerState);

		// Vertex Paint
		//unsigned int vertexColorID = modelComponent->VertexPaintColorID();

		// Render all meshes
		for (unsigned int i = 0; i < modelData.myMeshes.size(); ++i)
		{
			//if (vertexColorID > 0)
			//{
			//	auto vertexPaintMaterials = CMainSingleton::MaterialHandler().GetVertexPaintMaterials(modelComponent->VertexPaintMaterialNames());
			//	ID3D11Buffer* vertexColorBuffer = CMainSingleton::MaterialHandler().GetVertexColorBuffer(vertexColorID);
			//	UINT stride = sizeof(DirectX::SimpleMath::Vector3);
			//	ID3D11Buffer* bufferPointers[2] = { modelData.myMeshes[i].myVertexBuffer, vertexColorBuffer };
			//	UINT strides[2] = { modelData.myMeshes[i].myStride, stride };
			//	UINT offsets[2] = { 0, 0 };
			//	myContext->IASetInputLayout(myVertexPaintInputLayout);
			//	myContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
			//	myContext->VSSetShader(myVertexPaintModelVertexShader, nullptr, 0);
			//	myContext->PSSetShader(myVertexPaintPixelShader, nullptr, 0);
			//	myContext->PSSetShaderResources(12, 9, &vertexPaintMaterials[0]/*&myVertexPaintMaterials[0]*/);
			//}
			//else {
			//}
			myContext->IASetVertexBuffers(0, 1, &modelData.myMeshes[i].myVertexBuffer, &modelData.myMeshes[i].myStride, &modelData.myMeshes[i].myOffset);
			myContext->IASetIndexBuffer(modelData.myMeshes[i].myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(5, 4, &modelData.myMaterials[modelData.myMeshes[i].myMaterialIndex][0]);
			myContext->DrawIndexed(modelData.myMeshes[i].myNumberOfIndices, 0, 0);
			CRenderManager::myNumberOfDrawCallsThisFrame++;
		}
	}

	ID3D11ShaderResourceView* nullView = NULL;
	myContext->PSSetShaderResources(5,  1, &nullView);// Albedo
	myContext->PSSetShaderResources(6, 1, &nullView);// Normal
	myContext->PSSetShaderResources(7, 1, &nullView);// Material
	myContext->PSSetShaderResources(8, 1, &nullView);// DN1
	myContext->PSSetShaderResources(9, 1, &nullView);// DN2
	myContext->PSSetShaderResources(10, 1, &nullView);// DN3
	myContext->PSSetShaderResources(11, 1, &nullView);// DN4
	myObjectBufferData.myNumberOfDetailNormals = 0;// Making sure to reset it!
	
	for (auto& gameobject : aInstancedGameObjectList)
	{
		CInstancedModelComponent* instanceComponent = gameobject->GetComponent<CInstancedModelComponent>();
		if (instanceComponent == nullptr)
			continue;

		CModel* model = instanceComponent->GetModel();
		const CModel::SModelInstanceData& modelData = model->GetModelInstanceData();

		int dnCounter = 0;
		for (auto detailNormal : model->GetModelInstanceData().myDetailNormals)
		{
			if (detailNormal)
				++dnCounter;
		}
		myObjectBufferData.myNumberOfDetailNormals = dnCounter;

		BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

		{
			D3D11_MAPPED_SUBRESOURCE instanceBuffer;
			ZeroMemory(&instanceBuffer, sizeof(D3D11_MAPPED_SUBRESOURCE));
			ENGINE_HR_MESSAGE(myContext->Map(modelData.myInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceBuffer), "Instanced Buffer Could not be mapped in ForwardRenderer.");
			memcpy(instanceBuffer.pData, &instanceComponent->GetInstancedTransforms()[0], sizeof(DirectX::SimpleMath::Matrix) * instanceComponent->GetInstancedTransforms().size());
			myContext->Unmap(modelData.myInstanceBuffer, 0);
		}

		myContext->IASetPrimitiveTopology(modelData.myPrimitiveTopology);
		myContext->IASetInputLayout(modelData.myInputLayout);

		//ID3D11Buffer* bufferPointers[2] = {modelData.myVertexBuffer, modelData.myInstanceBuffer};
		//myContext->IASetVertexBuffers(0, 2, bufferPointers, modelData.myStride, modelData.myOffset);
		//myContext->IASetIndexBuffer(modelData.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(myInstancedModelVertexShader, nullptr, 0);

		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		//myContext->PSSetShaderResources(5, 3, &modelData.myTexture[0]);
		//myContext->PSSetShaderResources(9, 3, &modelData.myTexture[0]);
		myContext->PSSetShaderResources(8, 4, &modelData.myDetailNormals[0]);

		myContext->PSSetSamplers(0, 1, &modelData.mySamplerState);

		//myContext->PSSetShader(myGBufferPixelShader, nullptr, 0);
		myContext->PSSetShader(myCurrentGBufferPixelShader, nullptr, 0);

		// Render all meshes
		for (unsigned int i = 0; i < modelData.myMeshes.size(); ++i)
		{
			ID3D11Buffer* bufferPointers[2] = { modelData.myMeshes[i].myVertexBuffer, modelData.myInstanceBuffer };
			myContext->IASetVertexBuffers(0, 2, bufferPointers, modelData.myMeshes[i].myStride, modelData.myMeshes[i].myOffset);
			myContext->IASetIndexBuffer(modelData.myMeshes[i].myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(5, 3, &modelData.myMaterials[modelData.myMeshes[i].myMaterialIndex][0]);
			myContext->DrawIndexedInstanced(modelData.myMeshes[i].myNumberOfIndices, model->InstanceCount(), 0, 0, 0);
			CRenderManager::myNumberOfDrawCallsThisFrame++;
		}
	}

	myContext->PSSetShaderResources(5,  1, &nullView);// Albedo
	myContext->PSSetShaderResources(6, 1, &nullView);// Normal
	myContext->PSSetShaderResources(7, 1, &nullView);// Material
	myContext->PSSetShaderResources(8, 1, &nullView);// DN1
	myContext->PSSetShaderResources(9, 1, &nullView);// DN2
	myContext->PSSetShaderResources(10, 1, &nullView);// DN3
	myContext->PSSetShaderResources(11, 1, &nullView);// DN4
	myObjectBufferData.myNumberOfDetailNormals = 0;// Making sure to reset it!
}
//
//void CDeferredRenderer::Render(CCameraComponent* aCamera, CEnvironmentLight* anEnvironmentLight)
//{
//	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
//	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
//	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
//	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
//	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
//	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
//	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
//
//	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
//	ID3D11ShaderResourceView* environmentLightShaderResource = *anEnvironmentLight->GetCubeMap();
//	myContext->PSSetShaderResources(0, 1, &environmentLightShaderResource);
//
//	// Update lightbufferdata and fill lightbuffer
//	myLightBufferData.myDirectionalLightDirection = anEnvironmentLight->GetDirection();
//	myLightBufferData.myDirectionalLightColor = anEnvironmentLight->GetColor();
//	myLightBufferData.myDirectionalLightPosition = anEnvironmentLight->GetShadowPosition();
//	myLightBufferData.myToDirectionalLightView = anEnvironmentLight->GetShadowView();
//	myLightBufferData.myToDirectionalLightProjection = anEnvironmentLight->GetShadowProjection();
//	BindBuffer(myLightBuffer, myLightBufferData, "Light Buffer");
//	myContext->PSSetConstantBuffers(2, 1, &myLightBuffer);
//
//	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	myContext->IASetInputLayout(nullptr);
//	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
//	myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
//
//	myContext->GSSetShader(nullptr, nullptr, 0);
//
//	myContext->VSSetShader(myFullscreenShader, nullptr, 0);
//
//	if(myCurrentRenderPassShader)
//		myContext->PSSetShader(myRenderPassShaders[myRenderPassIndex], nullptr, 0);
//	else
//		myContext->PSSetShader(myEnvironmentLightShader, nullptr, 0);
//
//	myContext->PSSetSamplers(0, 1, &mySamplerState);
//	myContext->PSSetSamplers(1, 1, &myShadowSampler);
//
//	myContext->Draw(3, 0);
//	CRenderManager::myNumberOfDrawCallsThisFrame++;
//}
//
//void CDeferredRenderer::Render(CCameraComponent* aCamera, std::vector<CPointLight*>& aPointLightList)
//{
//	if (myCurrentRenderPassShader)
//		return;
//
//	if (aPointLightList.empty())
//		return;
//
//	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
//	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
//	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
//	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
//	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
//	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
//	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
//	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
//	myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
//	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
//
//	//myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	myContext->IASetInputLayout(myPointLightInputLayout);
//	myContext->IASetVertexBuffers(0, 1, &myPointLightVertexBuffer, &myPointLightStride, &myPointLightOffset);
//	myContext->IASetIndexBuffer(myPointLightIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	Matrix transform = {};
//	//UINT stride = sizeof(Vector4);
//	//UINT offset = 0;
//
//	for (CPointLight* currentInstance : aPointLightList) {
//		transform.Translation(currentInstance->GetPosition());
//		myObjectBufferData.myToWorld = transform;
//
//		BindBuffer(myObjectBuffer, myObjectBufferData, "Point Light Object Buffer");
//		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
//		
//		//Update pointlightbufferdata and fill pointlightbuffer
//		const SM::Vector3& position = currentInstance->GetPosition();
//		const SM::Vector3& color = currentInstance->GetColor();
//		myPointLightBufferData.myToWorldSpace = currentInstance->GetWorldMatrix();
//		myPointLightBufferData.myPositionAndRange = { position.x, position.y, position.z, currentInstance->GetRange() };
//		myPointLightBufferData.myColorAndIntensity = { color.x, color.y, color.z, currentInstance->GetIntensity() };
//
//		BindBuffer(myPointLightBuffer, myPointLightBufferData, "Point Light Buffer");
//		myContext->VSSetConstantBuffers(3, 1, &myPointLightBuffer);
//		myContext->PSSetConstantBuffers(3, 1, &myPointLightBuffer);
//		//myContext->GSSetConstantBuffers(3, 1, &myPointLightBuffer);
//
//		myContext->VSSetShader(myPointLightVertexShader, nullptr, 0);
//		//myContext->GSSetShader(myPointLightGeometryShader, nullptr, 0);
//
//		myContext->PSSetShader(myPointLightShader, nullptr, 0);
//		myContext->PSSetSamplers(0, 1, &mySamplerState);
//
//		//myContext->Draw(1, 0);
//		myContext->DrawIndexed(myPointLightNumberOfIndices, 0, 0);
//		CRenderManager::myNumberOfDrawCallsThisFrame++;
//	}
//
//	myContext->GSSetShader(nullptr, nullptr, 0);
//}
//
//void CDeferredRenderer::Render(CCameraComponent* aCamera, std::vector<CSpotLight*>& aSpotLightList) 
//{
//	if (myCurrentRenderPassShader)
//		return;
//
//	if (aSpotLightList.empty())
//		return;
//
//	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
//	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
//	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
//	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
//	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
//	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
//	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
//	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
//	myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
//	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
//
//	Matrix transform = {};
//	UINT stride = sizeof(Vector4);
//	UINT offset = 0;
//
//	for (CSpotLight* currentInstance : aSpotLightList) {
//		transform.Translation(currentInstance->GetPosition());
//		myObjectBufferData.myToWorld = transform;
//
//		BindBuffer(myObjectBuffer, myObjectBufferData, "Spot Light Object Buffer");
//		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
//
//		const SM::Vector3& position = currentInstance->GetPosition();
//		const SM::Vector3& color = currentInstance->GetColor();
//		const Vector3& direction = currentInstance->GetDirection();
//		mySpotLightBufferData.myToWorldSpace = currentInstance->GetWorldMatrix();
//		mySpotLightBufferData.myToViewSpace = currentInstance->GetViewMatrix();
//		mySpotLightBufferData.myToProjectionSpace = currentInstance->GetProjectionMatrix();
//		mySpotLightBufferData.myPositionAndRange = { position.x, position.y, position.z, currentInstance->GetRange() };
//		mySpotLightBufferData.myColorAndIntensity = { color.x, color.y, color.z, currentInstance->GetIntensity() };
//		mySpotLightBufferData.myDirectionAndAngleExponent = { direction.x, direction.y, direction.z, currentInstance->GetAngleExponent() };
//		mySpotLightBufferData.myDirectionNormal1 = currentInstance->GetDirectionNormal1();
//		mySpotLightBufferData.myDirectionNormal2 = currentInstance->GetDirectionNormal2();
//
//		BindBuffer(mySpotLightBuffer, mySpotLightBufferData, "Spot Light Buffer");
//		myContext->PSSetConstantBuffers(3, 1, &mySpotLightBuffer);
//		myContext->GSSetConstantBuffers(3, 1, &mySpotLightBuffer);
//
//		myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//		myContext->IASetInputLayout(myPointLightInputLayout);								// Probably fine, generate geometry in the same way
//		myContext->IASetVertexBuffers(0, 1, &myPointLightVertexBuffer, &stride, &offset);
//		myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
//
//		myContext->VSSetShader(mySpotLightVertexShader, nullptr, 0);
//		myContext->GSSetShader(mySpotLightGeometryShader, nullptr, 0);
//
//		myContext->PSSetShader(mySpotLightShader, nullptr, 0);
//		myContext->PSSetSamplers(0, 1, &mySamplerState);
//
//		myContext->Draw(1, 0);
//		CRenderManager::myNumberOfDrawCallsThisFrame++;
//	}
//
//	myContext->GSSetShader(nullptr, nullptr, 0);
//}
//
//void CDeferredRenderer::RenderVolumetricLight(CCameraComponent* aCamera, CEnvironmentLight* anEnvironmentLight)
//{
//	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
//	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
//	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
//	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
//	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
//	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
//	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
//
//	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
//
//	// Update lightbufferdata and fill lightbuffer
//	myLightBufferData.myDirectionalLightDirection = anEnvironmentLight->GetDirection();
//	myLightBufferData.myDirectionalLightColor = anEnvironmentLight->GetColor();
//	myLightBufferData.myDirectionalLightPosition = anEnvironmentLight->GetShadowPosition();
//	myLightBufferData.myToDirectionalLightView = anEnvironmentLight->GetShadowView();
//	myLightBufferData.myToDirectionalLightProjection = anEnvironmentLight->GetShadowProjection(); // Actual projection
//	BindBuffer(myLightBuffer, myLightBufferData, "Light Buffer");
//	myContext->PSSetConstantBuffers(1, 1, &myLightBuffer);
//
//	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	myContext->IASetInputLayout(nullptr);
//	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
//	myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
//
//	myContext->GSSetShader(nullptr, nullptr, 0);
//
//	myContext->VSSetShader(myFullscreenShader, nullptr, 0);
//
//	myContext->PSSetShader(myDirectionalVolumetricLightShader, nullptr, 0);
//
//	myContext->PSSetSamplers(0, 1, &mySamplerState);
//	myContext->PSSetSamplers(1, 1, &myShadowSampler);
//
//	myContext->Draw(3, 0);
//	CRenderManager::myNumberOfDrawCallsThisFrame++;
//}

void CDeferredRenderer::RenderSkybox(CCameraComponent* aCamera, CEnvironmentLight* anEnvironmentLight)
{
	mySkyboxTransformData.myCameraViewProjection = aCamera->GetViewMatrix() * aCamera->GetProjection();
	mySkyboxTransformData.myCameraViewProjection = mySkyboxTransformData.myCameraViewProjection.Transpose();
	BindBuffer(mySkyboxTransformBuffer, mySkyboxTransformData, "Skybox Transform Buffer");
	myContext->VSSetConstantBuffers(0, 1, &mySkyboxTransformBuffer);

	ID3D11ShaderResourceView* environmentLightShaderResource = *anEnvironmentLight->GetCubeMap();
	myContext->PSSetShaderResources(0, 1, &environmentLightShaderResource);

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(mySkyboxInputLayout);
	myContext->IASetVertexBuffers(0, 1, &mySkyboxVertexBuffer, &mySkyboxStride, &mySkyboxOffset);
	myContext->IASetIndexBuffer(mySkyboxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	myContext->GSSetShader(nullptr, nullptr, 0);

	myContext->VSSetShader(mySkyboxVertexShader, nullptr, 0);
	myContext->PSSetShader(mySkyboxPixelShader, nullptr, 0);

	myContext->PSSetSamplers(0, 1, &mySamplerState);

	myContext->DrawIndexed(mySkyboxNumberOfIndices, 0, 0);
	CRenderManager::myNumberOfDrawCallsThisFrame++;
}

bool CDeferredRenderer::LoadRenderPassPixelShaders(ID3D11Device* aDevice)
{
	// Render pass shaders
	std::ifstream psFile;
	psFile.open("Shaders/DeferredRenderPassShader_Albedo.cso", std::ios::binary);
	std::string psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();
	myRenderPassShaders.emplace_back();
	ENGINE_HR_MESSAGE(aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassShaders[0]), "Color Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/DeferredRenderPassShader_Normal.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();
	myRenderPassShaders.emplace_back();
	ENGINE_HR_MESSAGE(aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassShaders[1]), "Normal Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/DeferredRenderPassShader_Roughness.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();
	myRenderPassShaders.emplace_back();
	ENGINE_HR_MESSAGE(aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassShaders[2]), "Roughness Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/DeferredRenderPassShader_Metalness.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();
	myRenderPassShaders.emplace_back();
	ENGINE_HR_MESSAGE(aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassShaders[3]), "Metalness Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/DeferredRenderPassShader_AO.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();
	myRenderPassShaders.emplace_back();
	ENGINE_HR_MESSAGE(aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassShaders[4]), "Ambient Occlusion Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/DeferredRenderPassShader_Emissive.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();
	myRenderPassShaders.emplace_back();
	ENGINE_HR_MESSAGE(aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassShaders[5]), "Emissive Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/DeferredRenderPassGBufferPixelShader.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();
	ENGINE_HR_MESSAGE(aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassGBuffer), "Renderpass GBuffer could not be created.");

	return true;
}

bool CDeferredRenderer::ToggleRenderPass()
{
	++myRenderPassIndex;
	if (myRenderPassIndex == myRenderPassShaders.size())
	{
		myCurrentRenderPassShader = nullptr;
		return true;
	} else if (myRenderPassIndex > myRenderPassShaders.size())
	{
		myRenderPassIndex = 0;
	}
	myCurrentRenderPassShader = myRenderPassShaders[myRenderPassIndex];
	return false;
}
