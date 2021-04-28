#include "stdafx.h"
#include "FullscreenRenderer.h"
#include "DirectXFramework.h"
#include "RenderManager.h"
#include "Engine.h"
#include <fstream>

CFullscreenRenderer::CFullscreenRenderer() 
	: myContext(nullptr)
	, myVertexShader(nullptr)
	, myPixelShaders()
	, mySampler(nullptr)
{
}

CFullscreenRenderer::~CFullscreenRenderer() {
}

bool CFullscreenRenderer::Init(CDirectXFramework* aFramework) {
	if (!aFramework) {
		return false;
	}

	myContext = aFramework->GetContext();
	if (!myContext) {
		return false;
	}

	ID3D11Device* device = aFramework->GetDevice();
	HRESULT result;

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(SFullscreenData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myFullscreenDataBuffer), "Fullscreen Data Buffer could not be created.");

	std::ifstream vsFile;
	vsFile.open("Shaders/FullscreenVertexShader.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	if (FAILED(result)) {
		return false;
	}
	vsFile.close();
	myVertexShader = vertexShader;

	std::array<std::string, static_cast<size_t>(FullscreenShader::Count)> filepaths;
	filepaths[static_cast<size_t>(FullscreenShader::Copy)] = "Shaders/FullscreenPixelShader_Copy.cso";
	filepaths[static_cast<size_t>(FullscreenShader::CopyDepth)] = "Shaders/FullscreenPixelShader_CopyDepth.cso";
	filepaths[static_cast<size_t>(FullscreenShader::CopyGBuffer)] = "Shaders/FullscreenPixelShader_CopyGBuffer.cso";
	filepaths[static_cast<size_t>(FullscreenShader::Luminance)] = "Shaders/FullscreenPixelShader_Luminance.cso";
	filepaths[static_cast<size_t>(FullscreenShader::GaussianHorizontal)] = "Shaders/FullscreenPixelShader_GaussianBlurHorizontal.cso";
	filepaths[static_cast<size_t>(FullscreenShader::GaussianVertical)] = "Shaders/FullscreenPixelShader_GaussianBlurVertical.cso";
	filepaths[static_cast<size_t>(FullscreenShader::BilateralHorizontal)] = "Shaders/FullscreenPixelShader_BilateralBlurHorizontal.cso";
	filepaths[static_cast<size_t>(FullscreenShader::BilateralVertical)] = "Shaders/FullscreenPixelShader_BilateralBlurVertical.cso";
	filepaths[static_cast<size_t>(FullscreenShader::Bloom)] = "Shaders/FullscreenPixelShader_Bloom.cso";
	filepaths[static_cast<size_t>(FullscreenShader::Vignette)] = "Shaders/FullscreenPixelShader_Vignette.cso";
	filepaths[static_cast<size_t>(FullscreenShader::Tonemap)] = "Shaders/FullscreenPixelShader_Tonemap.cso";
	filepaths[static_cast<size_t>(FullscreenShader::GammaCorrection)] = "Shaders/FullscreenPixelShader_GammaCorrection.cso";
	filepaths[static_cast<size_t>(FullscreenShader::GammaCorrectionRenderPass)] = "Shaders/DeferredRenderPassFullscreenPixelShader_GammaCorrection.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FXAA)] = "Shaders/FullscreenPixelShader_FXAA.cso";
	filepaths[static_cast<size_t>(FullscreenShader::BrokenScreenEffect)] = "Shaders/FullscreenPixelShader_BrokenScreenEffect.cso";
	filepaths[static_cast<size_t>(FullscreenShader::DownsampleDepth)] = "Shaders/FullscreenPixelShader_DepthDownSample.cso";
	filepaths[static_cast<size_t>(FullscreenShader::DepthAwareUpsampling)] = "Shaders/FullscreenPixelShader_DepthAwareUpsample.cso";
	filepaths[static_cast<size_t>(FullscreenShader::DeferredAlbedo)] = "Shaders/DeferredRenderPassShader_Albedo.cso";
	filepaths[static_cast<size_t>(FullscreenShader::DeferredNormals)] = "Shaders/DeferredRenderPassShader_Normal.cso";
	filepaths[static_cast<size_t>(FullscreenShader::DeferredRoughness)] = "Shaders/DeferredRenderPassShader_Roughness.cso";
	filepaths[static_cast<size_t>(FullscreenShader::DeferredMetalness)] = "Shaders/DeferredRenderPassShader_Metalness.cso";
	filepaths[static_cast<size_t>(FullscreenShader::DeferredAmbientOcclusion)] = "Shaders/DeferredRenderPassShader_AO.cso";
	filepaths[static_cast<size_t>(FullscreenShader::DeferredEmissive)] = "Shaders/DeferredRenderPassShader_Emissive.cso";

	for (UINT i = 0; i < static_cast<size_t>(FullscreenShader::Count); i++) {
		std::ifstream psFile;
		psFile.open(filepaths[i], std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		ID3D11PixelShader* pixelShader;
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
		if (FAILED(result)) {
			return false;
		}
		psFile.close();
		myPixelShaders[i] = pixelShader;
	}

	//Start Sampler
	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 10;
	ENGINE_HR_MESSAGE(device->CreateSamplerState(&samplerDesc, &sampler), "Sampler could not be created.");
	mySampler = sampler;
	//End Sampler

	return true;
}

void CFullscreenRenderer::Render(FullscreenShader anEffect) {
	myFullscreenData.myResolution = IRONWROUGHT->GetWindowHandler()->GetResolution();
	BindBuffer(myFullscreenDataBuffer, myFullscreenData, "Fullscreen Data Buffer");
	
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myContext->IASetInputLayout(nullptr);
	myContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	myContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	myContext->VSSetShader(myVertexShader, nullptr, 0);
	myContext->PSSetShader(myPixelShaders[static_cast<size_t>(anEffect)], nullptr, 0);
	myContext->PSSetSamplers(0, 1, &mySampler);
	myContext->PSSetConstantBuffers(0, 1, &myFullscreenDataBuffer);

	myContext->Draw(3, 0);
	CRenderManager::myNumberOfDrawCallsThisFrame++;

	ID3D11ShaderResourceView* nullView = NULL;
	myContext->PSSetShaderResources(0, 1, &nullView);
	myContext->PSSetShaderResources(1, 1, &nullView);
	myContext->PSSetShaderResources(2, 1, &nullView);
	myContext->PSSetShaderResources(3, 1, &nullView);
	myContext->PSSetShaderResources(4, 1, &nullView);
	myContext->PSSetShaderResources(5, 1, &nullView);
	myContext->PSSetShaderResources(6, 1, &nullView);
	myContext->PSSetShaderResources(7, 1, &nullView);
	myContext->PSSetShaderResources(8, 1, &nullView);
	myContext->PSSetShaderResources(9, 1, &nullView);
	myContext->PSSetShaderResources(21, 1, &nullView);
	myContext->PSSetShaderResources(22, 1, &nullView);

	myContext->GSSetShader(nullptr, nullptr, 0);
}
