#include "stdafx.h"
#include "RenderManager.h"
#include "DirectXFramework.h"
#include "Scene.h"
#include "LineInstance.h"
#include "ModelFactory.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "ModelComponent.h"
#include "InstancedModelComponent.h"
#include "MainSingleton.h"
#include "PopupTextService.h"
#include "DialogueSystem.h"

#include "Engine.h"
#include "Scene.h"

#include "BoxLightComponent.h"
#include "BoxLight.h"

#include <algorithm>

unsigned int CRenderManager::myNumberOfDrawCallsThisFrame = 0;

CRenderManager::CRenderManager()
	: myDoFullRender(true)
	, myUseAntiAliasing(true)
	, myClearColor(0.5f, 0.5f, 0.5f, 1.0f)
{
}

CRenderManager::~CRenderManager()
{
}

bool CRenderManager::Init(CDirectXFramework* aFramework, CWindowHandler* aWindowHandler)
{
	ENGINE_ERROR_BOOL_MESSAGE(myForwardRenderer.Init(aFramework), "Failed to Init Forward Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myLightRenderer.Init(aFramework), "Failed to Init Light Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myDeferredRenderer.Init(aFramework), "Failed to Init Deferred Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myFullscreenRenderer.Init(aFramework), "Failed to Init Fullscreen Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myFullscreenTextureFactory.Init(aFramework), "Failed to Init Fullscreen Texture Factory.");
	ENGINE_ERROR_BOOL_MESSAGE(myParticleRenderer.Init(aFramework), "Failed to Init Particle Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myRenderStateManager.Init(aFramework), "Failed to Init Render State Manager.");
	ENGINE_ERROR_BOOL_MESSAGE(myVFXRenderer.Init(aFramework), "Failed to Init VFX Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(mySpriteRenderer.Init(aFramework), "Failed to Init Sprite Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myTextRenderer.Init(aFramework), "Failed to Init Text Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myShadowRenderer.Init(aFramework), "Failed to Init Shadow Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myDecalRenderer.Init(aFramework), "Failed to Init Decal Renderer.");

	ID3D11Texture2D* backbufferTexture = aFramework->GetBackbufferTexture();
	ENGINE_ERROR_BOOL_MESSAGE(backbufferTexture, "Backbuffer Texture is null.");

	myBackbuffer = myFullscreenTextureFactory.CreateTexture(backbufferTexture);
	InitRenderTextures(aWindowHandler);
	
	return true;
}

bool CRenderManager::ReInit(CDirectXFramework* aFramework, CWindowHandler* aWindowHandler)
{
	ENGINE_ERROR_BOOL_MESSAGE(myRenderStateManager.Init(aFramework), "Failed to Init Render State Manager.");

	ID3D11Texture2D* backbufferTexture = aFramework->GetBackbufferTexture();
	ENGINE_ERROR_BOOL_MESSAGE(backbufferTexture, "Backbuffer Texture is null.");

	myBackbuffer = myFullscreenTextureFactory.CreateTexture(backbufferTexture);
	InitRenderTextures(aWindowHandler);

	return true;
}

void CRenderManager::InitRenderTextures(CWindowHandler* aWindowHandler)
{
	myIntermediateDepth = myFullscreenTextureFactory.CreateDepth(aWindowHandler->GetResolution(), DXGI_FORMAT_R24G8_TYPELESS);
	myEnvironmentShadowDepth = myFullscreenTextureFactory.CreateDepth({ 2048.0f/* * 4.0f*/, 2048.0f/* * 4.0f*/ }, DXGI_FORMAT_R32_TYPELESS);
	myBoxLightShadowDepth = myFullscreenTextureFactory.CreateDepth(aWindowHandler->GetResolution(), DXGI_FORMAT_R32_TYPELESS);
	myDepthCopy = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R32_FLOAT);
	myDownsampledDepth = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution() / 2.0f, DXGI_FORMAT_R32_FLOAT);

	myIntermediateTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myLuminanceTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	myHalfSizeTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution() / 2.0f, DXGI_FORMAT_R16G16B16A16_FLOAT);
	myQuarterSizeTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution() / 4.0f, DXGI_FORMAT_R16G16B16A16_FLOAT);
	myBlurTexture1 = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	myBlurTexture2 = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	myVignetteTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	myDeferredLightingTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	myVolumetricAccumulationBuffer = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution() / 2.0f, DXGI_FORMAT_R16G16B16A16_FLOAT);
	myVolumetricBlurTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution() / 2.0f, DXGI_FORMAT_R16G16B16A16_FLOAT);
	myTonemappedTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	myAntiAliasedTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	myGBuffer = myFullscreenTextureFactory.CreateGBuffer(aWindowHandler->GetResolution());
	myGBufferCopy = myFullscreenTextureFactory.CreateGBuffer(aWindowHandler->GetResolution());
}

void CRenderManager::Render(CScene& aScene)
{
	CRenderManager::myNumberOfDrawCallsThisFrame = 0;

	if (Input::GetInstance()->IsKeyPressed(VK_F6))	
	{
		myDoFullRender = myDeferredRenderer.ToggleRenderPass();
		//myDoFullRender = myForwardRenderer.ToggleRenderPass();
	}

	myRenderStateManager.SetAllDefault();
	myBackbuffer.ClearTexture(myClearColor);
	myIntermediateTexture.ClearTexture(myClearColor);
	myIntermediateDepth.ClearDepth();
	myEnvironmentShadowDepth.ClearDepth();
	myGBuffer.ClearTextures(myClearColor);
	myDeferredLightingTexture.ClearTexture();
	myVolumetricAccumulationBuffer.ClearTexture();

	CEnvironmentLight* environmentlight = aScene.EnvironmentLight();
	CCameraComponent* maincamera = aScene.MainCamera();
	//CBoxLight* boxLight = aScene.CullBoxLights(nullptr)[0]; // For BoxLight shadow mapping

	std::vector<CGameObject*> gameObjects = aScene.CullGameObjects(maincamera);
	std::vector<CGameObject*> instancedGameObjects;
	std::vector<CGameObject*> instancedGameObjectsWithAlpha;
	std::vector<CGameObject*> gameObjectsWithAlpha;
	std::vector<int> indicesOfOutlineModels;
	std::vector<int> indicesOfAlphaGameObjects;

	for (unsigned int i = 0; i < gameObjects.size(); ++i)
	{
		auto instance = gameObjects[i];
		//for (auto gameObjectToOutline : aScene.ModelsToOutline()) {
		//	if (instance == gameObjectToOutline) {
		//		indicesOfOutlineModels.emplace_back(i);
		//	}
		//}

		if (instance->GetComponent<CInstancedModelComponent>()) 
		{
			if (instance->GetComponent<CInstancedModelComponent>()->RenderWithAlpha())
			{
				instancedGameObjectsWithAlpha.emplace_back(instance);
				indicesOfAlphaGameObjects.emplace_back(i);
				continue;
			}
			instancedGameObjects.emplace_back(instance);
		}
		else if (instance->GetComponent<CModelComponent>()) 
		{
			if (instance->GetComponent<CModelComponent>()->RenderWithAlpha())
			{
				gameObjectsWithAlpha.emplace_back(instance);
				indicesOfAlphaGameObjects.emplace_back(i);
				continue;
			}
		}
	}

	std::sort(indicesOfAlphaGameObjects.begin(), indicesOfAlphaGameObjects.end(), [](UINT a, UINT b) { return a > b; });
	for (auto index : indicesOfAlphaGameObjects)
	{
		std::swap(gameObjects[index], gameObjects.back());
		gameObjects.pop_back();
	}

	//std::sort(indicesOfOutlineModels.begin(), indicesOfOutlineModels.end(), [](UINT a, UINT b) { return a > b; });
	//for (auto index : indicesOfOutlineModels)
	//{
	//	std::swap(gameObjects[index], gameObjects.back());
	//	gameObjects.pop_back();
	//}

	// GBuffer
	myGBuffer.SetAsActiveTarget(&myIntermediateDepth);
	myDeferredRenderer.GenerateGBuffer(maincamera, gameObjects, instancedGameObjects);
	
	// Shadows
	myEnvironmentShadowDepth.SetAsDepthTarget();
	myShadowRenderer.Render(environmentlight, gameObjects, instancedGameObjects);
	//myBoxLightShadowDepth.SetAsDepthTarget();
	//myShadowRenderer.Render(boxLight, gameObjects, instancedGameObjects);

	// Decals
	myDepthCopy.SetAsActiveTarget();
	myIntermediateDepth.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPYDEPTH);
	//myGBufferCopy.SetAsActiveTarget();
	//myGBuffer.SetAllAsResources();
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPYGBUFFER);

	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_ONLYREAD);
	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_ALPHABLEND);
	myGBuffer.SetAsActiveTarget(&myIntermediateDepth);
	//myGBufferCopy.SetAllAsResources();
	myDepthCopy.SetAsResourceOnSlot(21);
	myDecalRenderer.Render(maincamera, gameObjects);

	// Lighting
	myDeferredLightingTexture.SetAsActiveTarget();
	myGBuffer.SetAllAsResources();
	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_ADDITIVEBLEND);
	std::vector<CPointLight*> onlyPointLights;
	onlyPointLights = aScene.CullPointLights(&maincamera->GameObject());
	std::vector<CSpotLight*> onlySpotLights;
	onlySpotLights = aScene.CullSpotLights(&maincamera->GameObject());
	std::vector<CBoxLight*> onlyBoxLights;
	onlyBoxLights = aScene.CullBoxLights(&maincamera->GameObject());

	myEnvironmentShadowDepth.SetAsResourceOnSlot(22);
	myLightRenderer.Render(maincamera, environmentlight);

	myRenderStateManager.SetRasterizerState(CRenderStateManager::RasterizerStates::RASTERIZERSTATE_NOFACECULLING);
	myLightRenderer.Render(maincamera, onlyPointLights);
	myLightRenderer.Render(maincamera, onlySpotLights);
	myLightRenderer.Render(maincamera, onlyBoxLights);

	// Volumetric Lighting
	//myVolumetricAccumulationBuffer.SetAsActiveTarget();

	//myLightRenderer.RenderVolumetric(maincamera, onlyPointLights);
	//myLightRenderer.RenderVolumetric(maincamera, onlySpotLights);
	//myBoxLightShadowDepth.SetAsResourceOnSlot(22);
	//myLightRenderer.RenderVolumetric(maincamera, onlyBoxLights);
	//myRenderStateManager.SetRasterizerState(CRenderStateManager::RasterizerStates::RASTERIZERSTATE_DEFAULT);
	//myEnvironmentShadowDepth.SetAsResourceOnSlot(22);
	//myLightRenderer.RenderVolumetric(maincamera, environmentlight);

	// Downsampling and Blur
	//myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_DISABLE);
	//myDownsampledDepth.SetAsActiveTarget();
	//myIntermediateDepth.SetAsResourceOnSlot(0);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_DOWNSAMPLE_DEPTH);

	//// Blur
	//myVolumetricBlurTexture.SetAsActiveTarget();
	//myVolumetricAccumulationBuffer.SetAsResourceOnSlot(0);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANHORIZONTAL);

	//myVolumetricAccumulationBuffer.SetAsActiveTarget();
	//myVolumetricBlurTexture.SetAsResourceOnSlot(0);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANVERTICAL);

	//myVolumetricBlurTexture.SetAsActiveTarget();
	//myVolumetricAccumulationBuffer.SetAsResourceOnSlot(0);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANHORIZONTAL);

	//myVolumetricAccumulationBuffer.SetAsActiveTarget();
	//myVolumetricBlurTexture.SetAsResourceOnSlot(0);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANVERTICAL);

	//myVolumetricBlurTexture.SetAsActiveTarget();
	//myVolumetricAccumulationBuffer.SetAsResourceOnSlot(0);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANHORIZONTAL);

	//myVolumetricAccumulationBuffer.SetAsActiveTarget();
	//myVolumetricBlurTexture.SetAsResourceOnSlot(0);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANVERTICAL);

	//myVolumetricBlurTexture.SetAsActiveTarget();
	//myVolumetricAccumulationBuffer.SetAsResourceOnSlot(0);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANHORIZONTAL);

	//myVolumetricAccumulationBuffer.SetAsActiveTarget();
	//myVolumetricBlurTexture.SetAsResourceOnSlot(0);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANVERTICAL);

	// Upsampling
	//myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_ADDITIVEBLEND);
	//myDeferredLightingTexture.SetAsActiveTarget();
	//myVolumetricAccumulationBuffer.SetAsResourceOnSlot(0);
	//myDownsampledDepth.SetAsResourceOnSlot(1);
	//myIntermediateDepth.SetAsResourceOnSlot(2);
	//myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_DEPTH_AWARE_UPSAMPLING);

	// Skybox
	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_DISABLE);
	myDeferredLightingTexture.SetAsActiveTarget(&myIntermediateDepth);

	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_DEPTHFIRST);
	myRenderStateManager.SetRasterizerState(CRenderStateManager::RasterizerStates::RASTERIZERSTATE_FRONTFACECULLING);
	myDeferredRenderer.RenderSkybox(maincamera, environmentlight);
	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_DEFAULT);
	myRenderStateManager.SetRasterizerState(CRenderStateManager::RasterizerStates::RASTERIZERSTATE_DEFAULT);

	// Render Lines
	const std::vector<CLineInstance*>& lineInstances = aScene.CullLineInstances();
	const std::vector<SLineTime>& lines = aScene.CullLines();
	myForwardRenderer.RenderLines(maincamera, lines);
	myForwardRenderer.RenderLineInstances(maincamera, lineInstances);

	// Alpha stage for objects in World 3D space
	//myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_ALPHABLEND);
	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_DISABLE);
	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_DEFAULT);
	//myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_ONLYREAD);

	std::vector<LightPair> pointlights;
	std::vector<LightPair> pointLightsInstanced;

	for (unsigned int i = 0; i < instancedGameObjectsWithAlpha.size(); ++i)
	{
		pointLightsInstanced.emplace_back(aScene.CullLightInstanced(instancedGameObjectsWithAlpha[i]->GetComponent<CInstancedModelComponent>()));
	}
	for (unsigned int i = 0; i < gameObjectsWithAlpha.size(); ++i)
	{
		pointlights.emplace_back(aScene.CullLights(gameObjectsWithAlpha[i]));
	}

	myForwardRenderer.InstancedRender(environmentlight, pointLightsInstanced, maincamera, instancedGameObjectsWithAlpha);
	myForwardRenderer.Render(environmentlight, pointlights, maincamera, gameObjectsWithAlpha);

	//VFX
	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_ALPHABLEND);
	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_ONLYREAD);
	myRenderStateManager.SetRasterizerState(CRenderStateManager::RasterizerStates::RASTERIZERSTATE_NOFACECULLING);
	myVFXRenderer.Render(maincamera, gameObjects);
	myRenderStateManager.SetRasterizerState(CRenderStateManager::RasterizerStates::RASTERIZERSTATE_DEFAULT);

	myParticleRenderer.Render(maincamera, gameObjects);

	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_DISABLE);
	//myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_DEFAULT);

	// Bloom
	myDoFullRender ? RenderBloom() : RenderWithoutBloom();

	// Tonemapping
	myTonemappedTexture.SetAsActiveTarget();
	myDeferredLightingTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_TONEMAP);

	if (INPUT->IsKeyPressed(VK_F2))
		myUseAntiAliasing = !myUseAntiAliasing;

	// Anti-aliasing
	if (myUseAntiAliasing)
	{
		myAntiAliasedTexture.SetAsActiveTarget();
		myTonemappedTexture.SetAsResourceOnSlot(0);
		myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_FXAA);

		myBackbuffer.SetAsActiveTarget();
		myAntiAliasedTexture.SetAsResourceOnSlot(0);
	}
	else
	{
		myBackbuffer.SetAsActiveTarget();
		myTonemappedTexture.SetAsResourceOnSlot(0);
	}

	// Gamma correction
	if (myDoFullRender)
		myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCRENSHADER_GAMMACORRECTION);
	else
		myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCRENSHADER_GAMMACORRECTION_RENDERPASS);

	// Sprites, animated UI
	myBackbuffer.SetAsActiveTarget();

	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_ALPHABLEND);
	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_ONLYREAD);

	std::vector<CSpriteInstance*> sprites = aScene.CullSprites();
	CMainSingleton::PopupTextService().EmplaceSprites(sprites);
	CMainSingleton::DialogueSystem().EmplaceSprites(sprites);
	mySpriteRenderer.Render(sprites);

	std::vector<CSpriteInstance*> animatedUIFrames;
	std::vector<CAnimatedUIElement*> animatedUIElements = aScene.CullAnimatedUI(animatedUIFrames);
	CEngine::GetInstance()->GetActiveScene().MainCamera()->EmplaceSprites(animatedUIFrames);
	mySpriteRenderer.Render(animatedUIElements);
	mySpriteRenderer.Render(animatedUIFrames);

	// Text
	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_DISABLE);
	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_DEFAULT);

	std::vector<CTextInstance*> textsToRender = aScene.Texts();
	CMainSingleton::PopupTextService().EmplaceTexts(textsToRender);
	CMainSingleton::DialogueSystem().EmplaceTexts(textsToRender);
	myTextRenderer.Render(textsToRender);
}

void CRenderManager::Release()
{
	Clear(myClearColor);
	CEngine::GetInstance()->myFramework->GetContext()->OMSetRenderTargets(0, 0, 0);
	CEngine::GetInstance()->myFramework->GetContext()->OMGetDepthStencilState(0, 0);
	CEngine::GetInstance()->myFramework->GetContext()->ClearState();

	myBackbuffer.ReleaseTexture();
	myIntermediateTexture.ReleaseTexture();
	myIntermediateDepth.ReleaseDepth();
	myLuminanceTexture.ReleaseTexture();
	myHalfSizeTexture.ReleaseTexture();
	myQuarterSizeTexture.ReleaseTexture();
	myBlurTexture1.ReleaseTexture();
	myBlurTexture2.ReleaseTexture();
	myVignetteTexture.ReleaseTexture();
	myDeferredLightingTexture.ReleaseTexture();
	//myGBuffer // Should something be released for the GBuffer?
}

void CRenderManager::Clear(DirectX::SimpleMath::Vector4 aClearColor)
{
	myBackbuffer.ClearTexture(aClearColor);
	myIntermediateDepth.ClearDepth();
}

void CRenderManager::RenderBloom()
{
	myHalfSizeTexture.SetAsActiveTarget();
	myDeferredLightingTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPY);

	myQuarterSizeTexture.SetAsActiveTarget();
	myHalfSizeTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPY);

	myBlurTexture1.SetAsActiveTarget();
	myQuarterSizeTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPY);

	myBlurTexture2.SetAsActiveTarget();
	myBlurTexture1.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANHORIZONTAL);

	myBlurTexture1.SetAsActiveTarget();
	myBlurTexture2.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANVERTICAL);

	myBlurTexture2.SetAsActiveTarget();
	myBlurTexture1.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANHORIZONTAL);

	myBlurTexture1.SetAsActiveTarget();
	myBlurTexture2.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_GAUSSIANVERTICAL);

	myQuarterSizeTexture.SetAsActiveTarget();
	myBlurTexture1.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPY);

	myHalfSizeTexture.SetAsActiveTarget();
	myQuarterSizeTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPY);

	myVignetteTexture.SetAsActiveTarget();
	myDeferredLightingTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_VIGNETTE);

	myDeferredLightingTexture.SetAsActiveTarget();
	myVignetteTexture.SetAsResourceOnSlot(0);
	myHalfSizeTexture.SetAsResourceOnSlot(1);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_BLOOM);
}

void CRenderManager::RenderWithoutBloom()
{
	myBackbuffer.SetAsActiveTarget();
	myIntermediateTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_VIGNETTE);
}
