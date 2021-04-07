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

	std::array<std::string, static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_COUNT)> filepaths;
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_COPY)] = "Shaders/FullscreenPixelShader_Copy.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_COPYDEPTH)] = "Shaders/FullscreenPixelShader_CopyDepth.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_COPYGBUFFER)] = "Shaders/FullscreenPixelShader_CopyGBuffer.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_LUMINANCE)] = "Shaders/FullscreenPixelShader_Luminance.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_GAUSSIANHORIZONTAL)] = "Shaders/FullscreenPixelShader_GaussianBlurHorizontal.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_GAUSSIANVERTICAL)] = "Shaders/FullscreenPixelShader_GaussianBlurVertical.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_BILATERALHORIZONTAL)] = "Shaders/FullscreenPixelShader_BilateralBlurHorizontal.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_BILATERALVERTICAL)] = "Shaders/FullscreenPixelShader_BilateralBlurVertical.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_BLOOM)] = "Shaders/FullscreenPixelShader_Bloom.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_VIGNETTE)] = "Shaders/FullscreenPixelShader_Vignette.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_TONEMAP)] = "Shaders/FullscreenPixelShader_Tonemap.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCRENSHADER_GAMMACORRECTION)] = "Shaders/FullscreenPixelShader_GammaCorrection.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCRENSHADER_GAMMACORRECTION_RENDERPASS)] = "Shaders/DeferredRenderPassFullscreenPixelShader_GammaCorrection.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_FXAA)] = "Shaders/FullscreenPixelShader_FXAA.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_DOWNSAMPLE_DEPTH)] = "Shaders/FullscreenPixelShader_DepthDownSample.cso";
	filepaths[static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_DEPTH_AWARE_UPSAMPLING)] = "Shaders/FullscreenPixelShader_DepthAwareUpsample.cso";

	for (UINT i = 0; i < static_cast<size_t>(FullscreenShader::FULLSCREENSHADER_COUNT); i++) {
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
