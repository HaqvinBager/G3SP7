#pragma once
#include "ForwardRenderer.h"
#include "DeferredRenderer.h"
#include "LightRenderer.h"
#include "FullscreenRenderer.h"
#include "FullscreenTexture.h"
#include "FullscreenTextureFactory.h"
#include "ParticleRenderer.h"
#include "VFXRenderer.h"
#include "RenderStateManager.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "ShadowRenderer.h"
#include "DecalRenderer.h"
#include "GBuffer.h"

class CDirectXFramework;
class CWindowHandler;
class CScene;

class CRenderManager {
public:
	CRenderManager();
	~CRenderManager();
	bool Init(CDirectXFramework* aFramework, CWindowHandler* aWindowHandler);
	bool ReInit(CDirectXFramework* aFramework, CWindowHandler* aWindowHandler);
	void Render(CScene& aScene);

	void Release();

public:
	static unsigned int myNumberOfDrawCallsThisFrame;

private:
	void Clear(DirectX::SimpleMath::Vector4 aClearColor);

private:
	void RenderBloom();
	void RenderWithoutBloom();

private:
	CRenderStateManager myRenderStateManager;
	CForwardRenderer myForwardRenderer;
	CDeferredRenderer myDeferredRenderer;
	CLightRenderer myLightRenderer;
	CFullscreenRenderer myFullscreenRenderer;
	CParticleRenderer myParticleRenderer;
	CVFXRenderer myVFXRenderer;
	CSpriteRenderer mySpriteRenderer;
	CTextRenderer myTextRenderer;
	CShadowRenderer myShadowRenderer;
	CDecalRenderer myDecalRenderer;

	CFullscreenTextureFactory myFullscreenTextureFactory;
	CFullscreenTexture myBackbuffer;
	CFullscreenTexture myIntermediateTexture;
	CFullscreenTexture myIntermediateDepth;
	CFullscreenTexture myEnvironmentShadowDepth;
	CFullscreenTexture myDepthCopy;
	CFullscreenTexture myLuminanceTexture;
	CFullscreenTexture myHalfSizeTexture;
	CFullscreenTexture myQuaterSizeTexture;
	CFullscreenTexture myBlurTexture1;
	CFullscreenTexture myBlurTexture2;
	CFullscreenTexture myVignetteTexture;
	CFullscreenTexture myDeferredLightingTexture;
	CFullscreenTexture myTonemappedTexture;
	CGBuffer myGBuffer;
	CGBuffer myGBufferCopy;

	DirectX::SimpleMath::Vector4 myClearColor;

	// Effectively used to toggle renderpasses and bloom. True == enable bloom, full render. False == disable bloom, isolated render pass
	bool myDoFullRender;
	//int myFrameCounter;// Used for a hack solution, can probably be removed
};