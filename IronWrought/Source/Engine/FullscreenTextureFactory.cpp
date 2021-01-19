#include "stdafx.h"
#include "FullscreenTextureFactory.h"
#include "FullscreenTexture.h"
#include "DirectXFramework.h"
#include "GBuffer.h"

CFullscreenTextureFactory::CFullscreenTextureFactory() : myFramework(nullptr) {
}

CFullscreenTextureFactory::~CFullscreenTextureFactory() {
}

bool CFullscreenTextureFactory::Init(CDirectXFramework* aFramework) {
	myFramework = aFramework;
	return true;
}

CFullscreenTexture CFullscreenTextureFactory::CreateTexture(SM::Vector2 aSize, DXGI_FORMAT aFormat) {
	HRESULT result;

	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = static_cast<unsigned int>(aSize.x);
	textureDesc.Height = static_cast<unsigned int>(aSize.y);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = aFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	result = myFramework->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &texture);
	if (FAILED(result)) {
		//return;
	}

	CFullscreenTexture returnTexture;
	returnTexture = CreateTexture(texture);

	ID3D11ShaderResourceView* shaderResource;
	result = myFramework->GetDevice()->CreateShaderResourceView(texture, nullptr, &shaderResource);
	if (FAILED(result)) {
		//return;
	}

	returnTexture.myShaderResource = shaderResource;
	return returnTexture;
}

CFullscreenTexture CFullscreenTextureFactory::CreateTexture(ID3D11Texture2D* aTexture) {
	HRESULT result;

	ID3D11RenderTargetView* renderTarget;
	result = myFramework->GetDevice()->CreateRenderTargetView(aTexture, nullptr, &renderTarget);
	if (FAILED(result)) {
		//return;
	}

	D3D11_VIEWPORT* viewport = nullptr;
	if (aTexture) {
		D3D11_TEXTURE2D_DESC textureDescription;
		aTexture->GetDesc(&textureDescription);
		viewport = new D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(textureDescription.Width), static_cast<float>(textureDescription.Height), 0.0f, 1.0f });
	}

	CFullscreenTexture returnTexture;
	returnTexture.myContext = myFramework->GetContext();
	returnTexture.myTexture = aTexture;
	returnTexture.myRenderTarget = renderTarget;
	returnTexture.myViewport = viewport;
	return returnTexture;
}

CFullscreenTexture CFullscreenTextureFactory::CreateDepth(SM::Vector2 aSize, DXGI_FORMAT aFormat) {
	HRESULT result;

	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = static_cast<unsigned int>(aSize.x);
	textureDesc.Height = static_cast<unsigned int>(aSize.y);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = aFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	result = myFramework->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &texture);
	if (FAILED(result)) {
		//return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	depthStencilViewDesc.Flags = 0;

	ID3D11DepthStencilView* depth;
	result = myFramework->GetDevice()->CreateDepthStencilView(texture, /*nullptr*/&depthStencilViewDesc, &depth);
	if (FAILED(result)) {
		//return;
	}

	D3D11_VIEWPORT* viewport = new D3D11_VIEWPORT({ 0.0f, 0.0f, aSize.x, aSize.y, 0.0f, 1.0f });

	CFullscreenTexture returnDepth;
	returnDepth.myContext = myFramework->GetContext();
	returnDepth.myTexture = texture;
	returnDepth.myDepth = depth;
	returnDepth.myViewport = viewport;
	return returnDepth;
}

CGBuffer CFullscreenTextureFactory::CreateGBuffer(DirectX::SimpleMath::Vector2 aSize)
{
	std::array<DXGI_FORMAT, static_cast<size_t>(CGBuffer::EGBufferTextures::COUNT)> textureFormats =
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8_UNORM,
	};

	//Creating textures, rendertargets, shaderresources and a viewport
	std::array<ID3D11Texture2D*, static_cast<size_t>(CGBuffer::EGBufferTextures::COUNT)> textures;
	std::array<ID3D11RenderTargetView*, static_cast<size_t>(CGBuffer::EGBufferTextures::COUNT)> renderTargets;
	std::array<ID3D11ShaderResourceView*, static_cast<size_t>(CGBuffer::EGBufferTextures::COUNT)> shaderResources;
	for (UINT i = 0; i < static_cast<size_t>(CGBuffer::EGBufferTextures::COUNT); ++i) {
		CFullscreenTexture texture = CreateTexture(aSize, textureFormats[i]);
		textures[i] = texture.myTexture;
		renderTargets[i] = texture.myRenderTarget;
		shaderResources[i] = texture.myShaderResource;
	}
	D3D11_VIEWPORT* viewport = new D3D11_VIEWPORT({ 0.0f, 0.0f, aSize.x, aSize.y, 0.0f, 1.0f });

	CGBuffer returnGBuffer;
	returnGBuffer.myContext = myFramework->GetContext();
	returnGBuffer.myTextures = std::move(textures);
	returnGBuffer.myRenderTargets = std::move(renderTargets);
	returnGBuffer.myShaderResources = std::move(shaderResources);
	returnGBuffer.myViewport = viewport;
	return returnGBuffer;
}
