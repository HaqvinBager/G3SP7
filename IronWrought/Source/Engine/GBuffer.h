#pragma once
#include "SimpleMath.h"
class CGBuffer
{
public:
	friend class CFullscreenTextureFactory;

public:
	enum class EGBufferTextures
	{
		POSITION,
		ALBEDO,
		NORMAL,
		VERTEXNORMAL,
		METALNESS,
		ROUGHNESS,
		AMBIENTOCCLUSION,
		EMISSIVE,
		COUNT
	};

public:
	CGBuffer();
	~CGBuffer();

	void ClearTextures(DirectX::SimpleMath::Vector4 aClearColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	void ReleaseRenderTargets();
	void SetAsActiveTarget(class CFullscreenTexture* aDepth = nullptr);
	void SetAsResourceOnSlot(EGBufferTextures aResource, UINT aSlot);
	void SetAllAsResources();

private:
	ID3D11DeviceContext* myContext;
	std::array<ID3D11Texture2D*, static_cast<size_t>(EGBufferTextures::COUNT)> myTextures;
	std::array<ID3D11RenderTargetView*, static_cast<size_t>(EGBufferTextures::COUNT)> myRenderTargets;
	std::array<ID3D11ShaderResourceView*, static_cast<size_t>(EGBufferTextures::COUNT)> myShaderResources;
	D3D11_VIEWPORT* myViewport;
};

