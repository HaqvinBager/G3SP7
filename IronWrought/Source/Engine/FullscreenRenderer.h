#pragma once
#include <array>
#include "EngineException.h"
#include <d3d11.h>
#include <SimpleMath.h>

struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
class CDirectXFramework;
class CRenderManager;

//namespace DirectX
//{
//	namespace SimpleMath
//	{
//		struct Vector2;
//		struct Vector4;
//	}
//}

class CFullscreenRenderer {
public:
	struct SPostProcessingBufferData
	{
		DirectX::SimpleMath::Vector4 myWhitePointColor;
		float myWhitePointIntensity;
		float myExposure;
		float mySSAORadius;
		float mySSAOSampleBias;
		float mySSAOMagnitude;
		float mySSAOContrast;
		
		int myIsReinhard;
		int myIsUncharted;
		int myIsACES;

		float myPadding[3];
	};

static_assert((sizeof(SPostProcessingBufferData) % 16) == 0, "CB size not padded correctly");

public:
	enum class FullscreenShader {
		Multiply,
		Copy,
		CopyDepth,
		CopyGBuffer,
		Luminance,
		GaussianHorizontal,
		GaussianVertical,
		BilateralHorizontal,
		BilateralVertical,
		Bloom,
		Vignette,
		Tonemap,
		GammaCorrection,
		GammaCorrectionRenderPass,
		FXAA,
		SSAO,
		SSAOBlur,
		BrokenScreenEffect,
		DownsampleDepth,
		DepthAwareUpsampling,
		DeferredAlbedo,
		DeferredNormals,
		DeferredRoughness,
		DeferredMetalness,
		DeferredAmbientOcclusion,
		DeferredEmissive,
		Count
	};

	friend CRenderManager;
	
private:
	static const unsigned int myKernelSize = 16;

private:
	template<class T>
	void BindBuffer(ID3D11Buffer* aBuffer, T& someBufferData, std::string aBufferType)
	{
		D3D11_MAPPED_SUBRESOURCE bufferData;
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		std::string errorMessage = aBufferType + " could not be bound.";
		ENGINE_HR_MESSAGE(myContext->Map(aBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData), errorMessage.c_str());

		memcpy(bufferData.pData, &someBufferData, sizeof(T));
		myContext->Unmap(aBuffer, 0);
	}

	struct SFullscreenData {
		DirectX::SimpleMath::Vector2 myResolution;
		DirectX::SimpleMath::Vector2 myNoiseScale;
		DirectX::SimpleMath::Vector4 mySampleKernel[myKernelSize];
	} myFullscreenData;

	static_assert((sizeof(SFullscreenData) % 16) == 0, "CB size not padded correctly");

	struct SFrameBufferData
	{
		DirectX::SimpleMath::Matrix myToCameraSpace;
		DirectX::SimpleMath::Matrix myToWorldFromCamera;
		DirectX::SimpleMath::Matrix myToProjectionSpace;
		DirectX::SimpleMath::Matrix myToCameraFromProjection;
		DirectX::SimpleMath::Vector4 myCameraPosition;
	} myFrameBufferData;

	static_assert((sizeof(SFrameBufferData) % 16) == 0, "CB size not padded correctly");

private:
	CFullscreenRenderer();
	~CFullscreenRenderer();
	bool Init(CDirectXFramework* aFramework);
	void Render(FullscreenShader aEffect);

	SPostProcessingBufferData myPostProcessingBufferData;

	ID3D11DeviceContext* myContext;
	ID3D11Buffer* myFullscreenDataBuffer;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myPostProcessingBuffer;
	ID3D11VertexShader* myVertexShader;
	ID3D11SamplerState* myClampSampler;
	ID3D11SamplerState* myWrapSampler;

	std::array<ID3D11PixelShader*, static_cast<size_t>(FullscreenShader::Count)> myPixelShaders;

	ID3D11ShaderResourceView* myNoiseTexture;
	DirectX::SimpleMath::Vector4 myKernel[myKernelSize];
};