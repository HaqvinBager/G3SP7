#include "stdafx.h"
#include "LightRenderer.h"
#include "DirectXFramework.h"
#include "RenderManager.h"
#include "GraphicsHelpers.h"

#include "Camera.h"
#include "EnvironmentLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "BoxLight.h"

#include "GameObject.h"
#include "CameraComponent.h"

CLightRenderer::CLightRenderer()
	: myContext(nullptr)
	, myFrameBuffer(nullptr)
	, myLightBuffer(nullptr)
	, myPointLightBuffer(nullptr)
	, mySpotLightBuffer(nullptr)
	, myBoxLightBuffer(nullptr)
	, myVolumetricLightBuffer(nullptr)
	, myPointLightVertexBuffer(nullptr)
	, myPointLightIndexBuffer(nullptr)
	, mySpotLightVertexBuffer(nullptr)
	, myBoxLightVertexBuffer(nullptr)
	, myBoxLightIndexBuffer(nullptr)
	, myFullscreenShader(nullptr)
	, myPointLightVertexShader(nullptr)
	, mySpotLightVertexShader(nullptr)
	, mySpotLightGeometryShader(nullptr)
	, myBoxLightVertexShader(nullptr)
	, myEnvironmentLightShader(nullptr)
	, myPointLightShader(nullptr)
	, mySpotLightShader(nullptr)
	, myBoxLightShader(nullptr)
	, myDirectionalVolumetricLightShader(nullptr)
	, myPointVolumetricLightShader(nullptr)
	, mySpotVolumetricLightShader(nullptr)
	, myBoxLightVolumetricLightShader(nullptr)
	, mySamplerState(nullptr)
	, myShadowSampler(nullptr)
	, myInputLayout(nullptr)
	, myPointLightNumberOfVertices(0)
	, myPointLightNumberOfIndices(0)
	, myPointLightStride(0)
	, myPointLightOffset(0)
{
}

CLightRenderer::~CLightRenderer()
{
}

bool CLightRenderer::Init(CDirectXFramework* aFramework)
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

	bufferDescription.ByteWidth = sizeof(SDirectionalLightBufferData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myLightBuffer), "Light Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SPointLightBufferData);
	ENGINE_HR_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myPointLightBuffer), "Point Light Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SSpotLightBufferData);
	ENGINE_HR_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &mySpotLightBuffer), "Spot Light Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SBoxLightBufferData);
	ENGINE_HR_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myBoxLightBuffer), "Box Light Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SVolumetricLightBufferData);
	ENGINE_HR_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myVolumetricLightBuffer), "Volumetric Light Buffer could not be created.");

	//Vertex Setup
	struct Vertex
	{
		float x, y, z, w;
	};

	Vertex pointLightVertices[24] = {
		// X      Y      Z      W 
		{ -1.0f, -1.0f, -1.0f,  1.0f },
		{  1.0f, -1.0f, -1.0f,  1.0f },
		{ -1.0f,  1.0f, -1.0f,  1.0f },
		{  1.0f,  1.0f, -1.0f,  1.0f },
		{ -1.0f, -1.0f,  1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f,  1.0f },
		{ -1.0f,  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f,  1.0f },
		// X      Y      Z      W 
		{ -1.0f, -1.0f, -1.0f,  1.0f },
		{ -1.0f,  1.0f, -1.0f,  1.0f },
		{ -1.0f, -1.0f,  1.0f,  1.0f },
		{ -1.0f,  1.0f,  1.0f,  1.0f },
		{  1.0f, -1.0f, -1.0f,  1.0f },
		{  1.0f,  1.0f, -1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f,  1.0f },
		// X      Y      Z      W 
		{ -1.0f, -1.0f, -1.0f,  1.0f },
		{  1.0f, -1.0f, -1.0f,  1.0f },
		{ -1.0f, -1.0f,  1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f,  1.0f },
		{ -1.0f,  1.0f, -1.0f,  1.0f },
		{  1.0f,  1.0f, -1.0f,  1.0f },
		{ -1.0f,  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f,  1.0f }
	};
	Vertex spotLightVertex[1] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Vertex boxLightVertices[24] = {
		// X      Y      Z      W 
		{ -0.5f, -0.5f,  0.0f,  1.0f },
		{  0.5f, -0.5f,  0.0f,  1.0f },
		{ -0.5f,  0.5f,  0.0f,  1.0f },
		{  0.5f,  0.5f,  0.0f,  1.0f },
		{ -0.5f, -0.5f,  1.0f,  1.0f },
		{  0.5f, -0.5f,  1.0f,  1.0f },
		{ -0.5f,  0.5f,  1.0f,  1.0f },
		{  0.5f,  0.5f,  1.0f,  1.0f },
		// X      Y      Z      W 
		{ -0.5f, -0.5f,  0.0f,  1.0f },
		{ -0.5f,  0.5f,  0.0f,  1.0f },
		{ -0.5f, -0.5f,  1.0f,  1.0f },
		{ -0.5f,  0.5f,  1.0f,  1.0f },
		{  0.5f, -0.5f,  0.0f,  1.0f },
		{  0.5f,  0.5f,  0.0f,  1.0f },
		{  0.5f, -0.5f,  1.0f,  1.0f },
		{  0.5f,  0.5f,  1.0f,  1.0f },
		// X      Y      Z      W 
		{ -0.5f, -0.5f,  0.0f,  1.0f },
		{  0.5f, -0.5f,  0.0f,  1.0f },
		{ -0.5f, -0.5f,  1.0f,  1.0f },
		{  0.5f, -0.5f,  1.0f,  1.0f },
		{ -0.5f,  0.5f,  0.0f,  1.0f },
		{  0.5f,  0.5f,  0.0f,  1.0f },
		{ -0.5f,  0.5f,  1.0f,  1.0f },
		{  0.5f,  0.5f,  1.0f,  1.0f }
	};

	//Index Setup
	unsigned int pointLightIndices[36] = {
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
	unsigned int boxLightIndices[36] = {
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

	// Point Light
#pragma region Point Light
	D3D11_BUFFER_DESC pointLightVertexBufferDesc = { 0 };
	pointLightVertexBufferDesc.ByteWidth = sizeof(pointLightVertices);
	pointLightVertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	pointLightVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA pointLightSubVertexResourceData = { 0 };
	pointLightSubVertexResourceData.pSysMem = pointLightVertices;

	ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateBuffer(&pointLightVertexBufferDesc, &pointLightSubVertexResourceData, &myPointLightVertexBuffer), "Point Light Vertex Buffer could not be created.");

	D3D11_BUFFER_DESC pointLightIndexBufferDesc = { 0 };
	pointLightIndexBufferDesc.ByteWidth = sizeof(pointLightIndices);
	pointLightIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	pointLightIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA pointLightIndexSubresourceData = { 0 };
	pointLightIndexSubresourceData.pSysMem = pointLightIndices;

	ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateBuffer(&pointLightIndexBufferDesc, &pointLightIndexSubresourceData, &myPointLightIndexBuffer), "Point Light Index Buffer could not be created.");

	myPointLightNumberOfVertices = static_cast<UINT>(sizeof(pointLightVertices) / sizeof(Vertex));
	myPointLightNumberOfIndices = static_cast<UINT>(sizeof(pointLightIndices) / sizeof(UINT));
	myPointLightStride = sizeof(Vertex);
	myPointLightOffset = 0;
#pragma endregion

	// Spot Light
#pragma region Spot Light
	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.ByteWidth = sizeof(Vertex);
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subVertexResourceData = { 0 };
	subVertexResourceData.pSysMem = spotLightVertex;

	ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateBuffer(&vertexBufferDesc, &subVertexResourceData, &mySpotLightVertexBuffer), "Spot Light Vertex Buffer could not be created.");
#pragma endregion

	// Box Light
#pragma region Box Light
	D3D11_BUFFER_DESC boxLightVertexBufferDesc = { 0 };
	boxLightVertexBufferDesc.ByteWidth = sizeof(boxLightVertices);
	boxLightVertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	boxLightVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA boxLightSubVertexResourceData = { 0 };
	boxLightSubVertexResourceData.pSysMem = boxLightVertices;

	ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateBuffer(&boxLightVertexBufferDesc, &boxLightSubVertexResourceData, &myBoxLightVertexBuffer), "Box Light Vertex Buffer could not be created.");

	D3D11_BUFFER_DESC boxLightIndexBufferDesc = { 0 };
	boxLightIndexBufferDesc.ByteWidth = sizeof(boxLightIndices);
	boxLightIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	boxLightIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA boxLightIndexSubresourceData = { 0 };
	boxLightIndexSubresourceData.pSysMem = boxLightIndices;

	ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateBuffer(&boxLightIndexBufferDesc, &boxLightIndexSubresourceData, &myBoxLightIndexBuffer), "Box Light Index Buffer could not be created.");
#pragma endregion

	std::string vsData;
	Graphics::CreateVertexShader("Shaders/DeferredVertexShader.cso", aFramework, &myFullscreenShader, vsData);

	Graphics::CreatePixelShader("Shaders/DeferredLightEnvironmentShader.cso", aFramework, &myEnvironmentLightShader);
	Graphics::CreatePixelShader("Shaders/DeferredLightPointShader.cso", aFramework, &myPointLightShader);
	Graphics::CreatePixelShader("Shaders/DeferredLightSpotShader.cso", aFramework, &mySpotLightShader);
	Graphics::CreatePixelShader("Shaders/DeferredLightBoxShader.cso", aFramework, &myBoxLightShader);
	Graphics::CreatePixelShader("Shaders/DirectionalVolumetricLightPixelShader.cso", aFramework, &myDirectionalVolumetricLightShader);
	Graphics::CreatePixelShader("Shaders/PointVolumetricLightPixelShader.cso", aFramework, &myPointVolumetricLightShader);
	Graphics::CreatePixelShader("Shaders/SpotVolumetricLightPixelShader.cso", aFramework, &mySpotVolumetricLightShader);
	Graphics::CreatePixelShader("Shaders/BoxVolumetricLightPixelShader.cso", aFramework, &myBoxLightVolumetricLightShader);

	// Point light 
	Graphics::CreateVertexShader("Shaders/PointLightVertexShader.cso", aFramework, &myPointLightVertexShader, vsData);
	D3D11_INPUT_ELEMENT_DESC pointLightLayout[] =
	{
		{"POSITION"	,	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ENGINE_HR_MESSAGE(aFramework->GetDevice()->CreateInputLayout(pointLightLayout, sizeof(pointLightLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &myInputLayout), "Input Layout could not be created.");

	// Spot light
	Graphics::CreateGeometryShader("Shaders/SpotLightGeometryShader.cso", aFramework, &mySpotLightGeometryShader);
	Graphics::CreateVertexShader("Shaders/SpotLightVertexShader.cso", aFramework, &mySpotLightVertexShader, vsData);

	// Box light
	Graphics::CreateVertexShader("Shaders/BoxLightVertexShader.cso", aFramework, &myBoxLightVertexShader, vsData);

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
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 10;

	ENGINE_HR_BOOL_MESSAGE(device->CreateSamplerState(&samplerDesc, &myShadowSampler), "Shadow Sampler could not be created.");

	return true;
}

void CLightRenderer::Render(CCameraComponent* aCamera, CEnvironmentLight* anEnvironmentLight)
{
	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
	ID3D11ShaderResourceView* environmentLightShaderResource = *anEnvironmentLight->GetCubeMap();
	myContext->PSSetShaderResources(0, 1, &environmentLightShaderResource);

	// Update lightbufferdata and fill lightbuffer
	myDirectionalLightBufferData.myDirectionalLightDirection = anEnvironmentLight->GetDirection();
	myDirectionalLightBufferData.myDirectionalLightColor = anEnvironmentLight->GetColor();
	myDirectionalLightBufferData.myDirectionalLightPosition = anEnvironmentLight->GetShadowPosition();
	myDirectionalLightBufferData.myToDirectionalLightView = anEnvironmentLight->GetShadowView();
	myDirectionalLightBufferData.myToDirectionalLightProjection = anEnvironmentLight->GetShadowProjection();
	myDirectionalLightBufferData.myDirectionalLightShadowMapResolution = { 2048.0f * 4.0f, 2048.0f * 4.0f };
	BindBuffer(myLightBuffer, myDirectionalLightBufferData, "Light Buffer");
	myContext->PSSetConstantBuffers(2, 1, &myLightBuffer);

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(nullptr);
	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	myContext->GSSetShader(nullptr, nullptr, 0);

	myContext->VSSetShader(myFullscreenShader, nullptr, 0);

	myContext->PSSetShader(myEnvironmentLightShader, nullptr, 0);

	myContext->PSSetSamplers(0, 1, &mySamplerState);
	myContext->PSSetSamplers(1, 1, &myShadowSampler);

	myContext->Draw(3, 0);
	CRenderManager::myNumberOfDrawCallsThisFrame++;
}

void CLightRenderer::Render(CCameraComponent* aCamera, std::vector<CPointLight*>& aPointLightList)
{
	if (aPointLightList.empty())
		return;

	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(myInputLayout);
	myContext->IASetVertexBuffers(0, 1, &myPointLightVertexBuffer, &myPointLightStride, &myPointLightOffset);
	myContext->IASetIndexBuffer(myPointLightIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	for (CPointLight* currentInstance : aPointLightList) {
		const SM::Vector3& position = currentInstance->GetPosition();
		const SM::Vector3& color = currentInstance->GetColor();
		myPointLightBufferData.myToWorldSpace = currentInstance->GetWorldMatrix();
		myPointLightBufferData.myPositionAndRange = { position.x, position.y, position.z, currentInstance->GetRange() };
		myPointLightBufferData.myColorAndIntensity = { color.x, color.y, color.z, currentInstance->GetIntensity() };

		BindBuffer(myPointLightBuffer, myPointLightBufferData, "Point Light Buffer");
		myContext->VSSetConstantBuffers(3, 1, &myPointLightBuffer);
		myContext->PSSetConstantBuffers(3, 1, &myPointLightBuffer);

		myContext->VSSetShader(myPointLightVertexShader, nullptr, 0);

		myContext->PSSetShader(myPointLightShader, nullptr, 0);
		myContext->PSSetSamplers(0, 1, &mySamplerState);

		myContext->DrawIndexed(myPointLightNumberOfIndices, 0, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}
}

void CLightRenderer::Render(CCameraComponent* aCamera, std::vector<CSpotLight*>& aSpotLightList)
{
	if (aSpotLightList.empty())
		return;

	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	UINT stride = sizeof(Vector4);
	UINT offset = 0;

	for (CSpotLight* currentInstance : aSpotLightList) {
		const SM::Vector3& position = currentInstance->GetPosition();
		const SM::Vector3& color = currentInstance->GetColor();
		const Vector3& direction = currentInstance->GetDirection();
		mySpotLightBufferData.myToWorldSpace = currentInstance->GetWorldMatrix();
		mySpotLightBufferData.myToViewSpace = currentInstance->GetViewMatrix();
		mySpotLightBufferData.myToProjectionSpace = currentInstance->GetProjectionMatrix();
		mySpotLightBufferData.myPositionAndRange = { position.x, position.y, position.z, currentInstance->GetRange() };
		mySpotLightBufferData.myColorAndIntensity = { color.x, color.y, color.z, currentInstance->GetIntensity() };
		mySpotLightBufferData.myDirectionAndAngleExponent = { direction.x, direction.y, direction.z, currentInstance->GetAngleExponent() };
		mySpotLightBufferData.myDirectionNormal1 = currentInstance->GetDirectionNormal1();
		mySpotLightBufferData.myDirectionNormal2 = currentInstance->GetDirectionNormal2();

		BindBuffer(mySpotLightBuffer, mySpotLightBufferData, "Spot Light Buffer");
		myContext->PSSetConstantBuffers(3, 1, &mySpotLightBuffer);
		myContext->GSSetConstantBuffers(3, 1, &mySpotLightBuffer);

		myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		myContext->IASetInputLayout(myInputLayout);
		myContext->IASetVertexBuffers(0, 1, &myPointLightVertexBuffer, &stride, &offset);
		myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

		myContext->VSSetShader(mySpotLightVertexShader, nullptr, 0);
		myContext->GSSetShader(mySpotLightGeometryShader, nullptr, 0);

		myContext->PSSetShader(mySpotLightShader, nullptr, 0);
		myContext->PSSetSamplers(0, 1, &mySamplerState);

		myContext->Draw(1, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}

	myContext->GSSetShader(nullptr, nullptr, 0);
}

void CLightRenderer::Render(CCameraComponent* aCamera, std::vector<CBoxLight*>& aBoxLightList)
{
	if (aBoxLightList.empty())
		return;

	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(myInputLayout);
	myContext->IASetVertexBuffers(0, 1, &myBoxLightVertexBuffer, &myPointLightStride, &myPointLightOffset);
	myContext->IASetIndexBuffer(myBoxLightIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	for (CBoxLight* currentInstance : aBoxLightList) {
		const Vector3& position = currentInstance->GetPosition();
		const Vector3& color = currentInstance->GetColor();
		const Vector3& direction = currentInstance->GetDirection();
		myBoxLightBufferData.myToWorldSpace = currentInstance->GetWorldMatrix();
		myBoxLightBufferData.myToViewSpace = currentInstance->GetViewMatrix();
		myBoxLightBufferData.myToProjectionSpace = currentInstance->GetProjectionMatrix();
		myBoxLightBufferData.myToObjectSpace = currentInstance->GetWorldMatrix().Invert();
		myBoxLightBufferData.myColorAndIntensity = { color.x, color.y, color.z, currentInstance->GetIntensity() };
		myBoxLightBufferData.myPositionAndRange = { position.x, position.y, position.z, currentInstance->GetRange() };
		myBoxLightBufferData.myDirection = { direction.x, direction.y, direction.z, 0.0f };
		myBoxLightBufferData.myDirectionNormal1 = currentInstance->GetDirectionNormal1();
		myBoxLightBufferData.myDirectionNormal2 = currentInstance->GetDirectionNormal2();
		myBoxLightBufferData.myWidthAndHeight = { currentInstance->GetWidth(), currentInstance->GetHeight() };

		BindBuffer(myBoxLightBuffer, myBoxLightBufferData, "Box Light Buffer");
		myContext->VSSetConstantBuffers(3, 1, &myBoxLightBuffer);
		myContext->PSSetConstantBuffers(3, 1, &myBoxLightBuffer);

		myContext->VSSetShader(myBoxLightVertexShader, nullptr, 0);

		myContext->PSSetShader(myBoxLightShader, nullptr, 0);
		myContext->PSSetSamplers(0, 1, &mySamplerState);

		myContext->DrawIndexed(myPointLightNumberOfIndices, 0, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}
}

void CLightRenderer::RenderVolumetric(CCameraComponent* aCamera, CEnvironmentLight* anEnvironmentLight)
{
	if (!anEnvironmentLight->GetIsVolumetric())
		return;

	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	// Update lightbufferdata and fill lightbuffer
	myDirectionalLightBufferData.myDirectionalLightDirection = anEnvironmentLight->GetDirection();
	myDirectionalLightBufferData.myDirectionalLightColor = anEnvironmentLight->GetColor();
	myDirectionalLightBufferData.myDirectionalLightPosition = anEnvironmentLight->GetShadowPosition();
	myDirectionalLightBufferData.myToDirectionalLightView = anEnvironmentLight->GetShadowView();
	myDirectionalLightBufferData.myToDirectionalLightProjection = anEnvironmentLight->GetShadowProjection(); // Actual projection
	myDirectionalLightBufferData.myDirectionalLightShadowMapResolution = anEnvironmentLight->GetShadowmapResolution();
	BindBuffer(myLightBuffer, myDirectionalLightBufferData, "Light Buffer");
	myContext->PSSetConstantBuffers(1, 1, &myLightBuffer);

	myVolumetricLightBufferData.myNumberOfSamplesReciprocal = (1.0f / anEnvironmentLight->GetNumberOfSamples());
	myVolumetricLightBufferData.myLightPower = anEnvironmentLight->GetLightPower();
	myVolumetricLightBufferData.myScatteringProbability = anEnvironmentLight->GetScatteringProbability();
	myVolumetricLightBufferData.myHenyeyGreensteinGValue = anEnvironmentLight->GetHenyeyGreensteinGValue();

	BindBuffer(myVolumetricLightBuffer, myVolumetricLightBufferData, "Volumetric Light Buffer");
	myContext->PSSetConstantBuffers(4, 1, &myVolumetricLightBuffer);

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(nullptr);
	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	myContext->GSSetShader(nullptr, nullptr, 0);

	myContext->VSSetShader(myFullscreenShader, nullptr, 0);

	myContext->PSSetShader(myDirectionalVolumetricLightShader, nullptr, 0);

	myContext->PSSetSamplers(0, 1, &mySamplerState);
	myContext->PSSetSamplers(1, 1, &myShadowSampler);

	myContext->Draw(3, 0);
	CRenderManager::myNumberOfDrawCallsThisFrame++;
}

void CLightRenderer::RenderVolumetric(CCameraComponent* aCamera, std::vector<CPointLight*>& aPointLightList)
{
	if (aPointLightList.empty())
		return;

	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(myInputLayout);
	myContext->IASetVertexBuffers(0, 1, &myPointLightVertexBuffer, &myPointLightStride, &myPointLightOffset);
	myContext->IASetIndexBuffer(myPointLightIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	for (CPointLight* currentInstance : aPointLightList) {
		const SM::Vector3& position = currentInstance->GetPosition();
		const SM::Vector3& color = currentInstance->GetColor();
		myPointLightBufferData.myToWorldSpace = currentInstance->GetWorldMatrix();
		myPointLightBufferData.myPositionAndRange = { position.x, position.y, position.z, currentInstance->GetRange() };
		myPointLightBufferData.myColorAndIntensity = { color.x, color.y, color.z, currentInstance->GetIntensity() };

		BindBuffer(myPointLightBuffer, myPointLightBufferData, "Point Light Buffer");
		myContext->VSSetConstantBuffers(3, 1, &myPointLightBuffer);
		myContext->PSSetConstantBuffers(3, 1, &myPointLightBuffer);

		myVolumetricLightBufferData.myNumberOfSamplesReciprocal = (1.0f / 16.0f);
		myVolumetricLightBufferData.myLightPower = 1000000.0f;
		myVolumetricLightBufferData.myScatteringProbability = 0.0001f;
		myVolumetricLightBufferData.myHenyeyGreensteinGValue = 0.0f;

		BindBuffer(myVolumetricLightBuffer, myVolumetricLightBufferData, "Volumetric Light Buffer");
		myContext->PSSetConstantBuffers(4, 1, &myVolumetricLightBuffer);

		myContext->VSSetShader(myPointLightVertexShader, nullptr, 0);

		myContext->PSSetShader(myPointVolumetricLightShader, nullptr, 0);
		myContext->PSSetSamplers(0, 1, &mySamplerState);

		myContext->DrawIndexed(myPointLightNumberOfIndices, 0, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}
}

void CLightRenderer::RenderVolumetric(CCameraComponent* aCamera, std::vector<CSpotLight*>& aSpotLightList)
{
	if (aSpotLightList.empty())
		return;

	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	UINT stride = sizeof(Vector4);
	UINT offset = 0;

	for (CSpotLight* currentInstance : aSpotLightList) {
		const SM::Vector3& position = currentInstance->GetPosition();
		const SM::Vector3& color = currentInstance->GetColor();
		const Vector3& direction = currentInstance->GetDirection();

		mySpotLightBufferData.myToWorldSpace = currentInstance->GetWorldMatrix();
		mySpotLightBufferData.myToViewSpace = currentInstance->GetViewMatrix();
		mySpotLightBufferData.myToProjectionSpace = currentInstance->GetProjectionMatrix();
		mySpotLightBufferData.myPositionAndRange = { position.x, position.y, position.z, currentInstance->GetRange() };
		mySpotLightBufferData.myColorAndIntensity = { color.x, color.y, color.z, currentInstance->GetIntensity() };
		mySpotLightBufferData.myDirectionAndAngleExponent = { direction.x, direction.y, direction.z, currentInstance->GetAngleExponent() };
		mySpotLightBufferData.myDirectionNormal1 = currentInstance->GetDirectionNormal1();
		mySpotLightBufferData.myDirectionNormal2 = currentInstance->GetDirectionNormal2();

		mySpotLightBufferData.myUpLeftCorner = currentInstance->GetUpLeftCorner();
		mySpotLightBufferData.myUpRightCorner = currentInstance->GetUpRightCorner();
		mySpotLightBufferData.myDownLeftCorner = currentInstance->GetDownLeftCorner();
		mySpotLightBufferData.myDownRightCorner = currentInstance->GetDownRightCorner();

		BindBuffer(mySpotLightBuffer, mySpotLightBufferData, "Spot Light Buffer");
		myContext->PSSetConstantBuffers(3, 1, &mySpotLightBuffer);
		myContext->GSSetConstantBuffers(3, 1, &mySpotLightBuffer);

		myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		myContext->IASetInputLayout(myInputLayout);
		myContext->IASetVertexBuffers(0, 1, &mySpotLightVertexBuffer, &stride, &offset);
		myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

		myContext->VSSetShader(mySpotLightVertexShader, nullptr, 0);
		myContext->GSSetShader(mySpotLightGeometryShader, nullptr, 0);

		myContext->PSSetShader(mySpotVolumetricLightShader, nullptr, 0);
		myContext->PSSetSamplers(0, 1, &mySamplerState);

		myContext->Draw(1, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}

	myContext->GSSetShader(nullptr, nullptr, 0);
}

void CLightRenderer::RenderVolumetric(CCameraComponent* aCamera, std::vector<CBoxLight*>& aBoxLightList)
{
	if (aBoxLightList.empty())
		return;

	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToWorldFromCamera = cameraMatrix;
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myToCameraFromProjection = aCamera->GetProjection().Invert();
	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(myInputLayout);
	myContext->IASetVertexBuffers(0, 1, &myBoxLightVertexBuffer, &myPointLightStride, &myPointLightOffset);
	myContext->IASetIndexBuffer(myBoxLightIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	for (CBoxLight* currentInstance : aBoxLightList) {
		const Vector3& position = currentInstance->GetPosition();
		const Vector3& color = currentInstance->GetColor();
		const Vector3& direction = currentInstance->GetDirection();
		myBoxLightBufferData.myToWorldSpace = currentInstance->GetWorldMatrix();
		myBoxLightBufferData.myToViewSpace = currentInstance->GetViewMatrix();
		myBoxLightBufferData.myToProjectionSpace = currentInstance->GetProjectionMatrix();
		myBoxLightBufferData.myToObjectSpace = currentInstance->GetWorldMatrix().Invert();
		myBoxLightBufferData.myColorAndIntensity = { color.x, color.y, color.z, currentInstance->GetIntensity() };
		myBoxLightBufferData.myPositionAndRange = { position.x, position.y, position.z, currentInstance->GetRange() };
		myBoxLightBufferData.myDirection = { direction.x, direction.y, direction.z, 0.0f };
		myBoxLightBufferData.myDirectionNormal1 = currentInstance->GetDirectionNormal1();
		myBoxLightBufferData.myDirectionNormal2 = currentInstance->GetDirectionNormal2();
		myBoxLightBufferData.myWidthAndHeight = { currentInstance->GetWidth(), currentInstance->GetHeight() };

		BindBuffer(myBoxLightBuffer, myBoxLightBufferData, "Box Light Buffer");
		myContext->VSSetConstantBuffers(3, 1, &myBoxLightBuffer);
		myContext->PSSetConstantBuffers(3, 1, &myBoxLightBuffer);

		myVolumetricLightBufferData.myNumberOfSamplesReciprocal = (1.0f / 16.0f);
		myVolumetricLightBufferData.myLightPower = 100000.0f;
		myVolumetricLightBufferData.myScatteringProbability = 0.0001f;
		myVolumetricLightBufferData.myHenyeyGreensteinGValue = 0.0f;

		BindBuffer(myVolumetricLightBuffer, myVolumetricLightBufferData, "Volumetric Light Buffer");
		myContext->PSSetConstantBuffers(4, 1, &myVolumetricLightBuffer);

		//myContext->VSSetShader(myFullscreenShader, nullptr, 0);
		myContext->VSSetShader(myBoxLightVertexShader, nullptr, 0);

		myContext->PSSetShader(myBoxLightVolumetricLightShader, nullptr, 0);
		myContext->PSSetSamplers(0, 1, &mySamplerState);

		myContext->DrawIndexed(myPointLightNumberOfIndices, 0, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}
}
