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

#include <algorithm>

#define USING_DEFERRED

CRenderManager::CRenderManager() /*: myScene(*CScene::GetInstance())*/
	: myUseBloom(true)
	, myClearColor(0.8f, 0.5f, 0.5f, 1.0f)
{
}

CRenderManager::~CRenderManager()
{
}

bool CRenderManager::Init(CDirectXFramework* aFramework, CWindowHandler* aWindowHandler)
{
	ENGINE_ERROR_BOOL_MESSAGE(myForwardRenderer.Init(aFramework), "Failed to Init Forward Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myDeferredRenderer.Init(aFramework), "Failed to Init Deferred Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myFullscreenRenderer.Init(aFramework), "Failed to Init Fullscreen Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myFullscreenTextureFactory.Init(aFramework), "Failed to Init Fullscreen Texture Factory.");
	ENGINE_ERROR_BOOL_MESSAGE(myParticleRenderer.Init(aFramework), "Failed to Init Particle Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myRenderStateManager.Init(aFramework), "Failed to Init Render State Manager.");
	ENGINE_ERROR_BOOL_MESSAGE(myVFXRenderer.Init(aFramework), "Failed to Init VFX Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(mySpriteRenderer.Init(aFramework), "Failed to Init Sprite Renderer.");
	ENGINE_ERROR_BOOL_MESSAGE(myTextRenderer.Init(aFramework), "Failed to Init Text Renderer.");

	ID3D11Texture2D* backbufferTexture = aFramework->GetBackbufferTexture();
	if (!backbufferTexture)
	{
		return false;
	}
	myBackbuffer = myFullscreenTextureFactory.CreateTexture(backbufferTexture);
	myIntermediateDepth = myFullscreenTextureFactory.CreateDepth(aWindowHandler->GetResolution(), DXGI_FORMAT_D24_UNORM_S8_UINT);

	myIntermediateTexture	= myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myLuminanceTexture		= myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myHalfSizeTexture		= myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution() / 2.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
	myQuaterSizeTexture		= myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution() / 4.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
	myBlurTexture1			= myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myBlurTexture2			= myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myVignetteTexture		= myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myDeferredTexture		= myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R32G32B32A32_FLOAT);
	myGBuffer				= myFullscreenTextureFactory.CreateGBuffer(aWindowHandler->GetResolution());

	return true;
}

bool CRenderManager::ReInit(CDirectXFramework* aFramework, CWindowHandler* aWindowHandler)
{

	if (!myRenderStateManager.Init(aFramework))
	{
		return false;
	}

	ID3D11Texture2D* backbufferTexture = aFramework->GetBackbufferTexture();
	if (!backbufferTexture)
	{
		return false;
	}
	myBackbuffer = myFullscreenTextureFactory.CreateTexture(backbufferTexture);
	myIntermediateDepth = myFullscreenTextureFactory.CreateDepth(aWindowHandler->GetResolution(), DXGI_FORMAT_D24_UNORM_S8_UINT);

	myIntermediateTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myLuminanceTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myHalfSizeTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution() / 2.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
	myQuaterSizeTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution() / 4.0f, DXGI_FORMAT_R8G8B8A8_UNORM);
	myBlurTexture1 = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myBlurTexture2 = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myVignetteTexture = myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM);
	myDeferredTexture		= myFullscreenTextureFactory.CreateTexture(aWindowHandler->GetResolution(), DXGI_FORMAT_R32G32B32A32_FLOAT);
	myGBuffer				= myFullscreenTextureFactory.CreateGBuffer(aWindowHandler->GetResolution());
	
	return true;
}

void CRenderManager::Render(CScene& aScene)
{
	if (Input::GetInstance()->IsKeyPressed(VK_F6))	
	{
#ifdef USING_DEFERRED //Define found under #includes
		myUseBloom = myDeferredRenderer.ToggleRenderPass();
#else
		myUseBloom = myForwardRenderer.ToggleRenderPass();
#endif
	}

	myRenderStateManager.SetAllDefault();
	myBackbuffer.ClearTexture(myClearColor);
	myIntermediateTexture.ClearTexture(myClearColor);
	myIntermediateDepth.ClearDepth();
	myGBuffer.ClearTextures(myClearColor);
	myDeferredTexture.ClearTexture();

	CEnvironmentLight* environmentlight = aScene.GetEnvironmentLight();
	CCameraComponent* maincamera = aScene.GetMainCamera();

	std::vector<CGameObject*> gameObjects = aScene.CullGameObjects(maincamera);
	std::vector<CGameObject*> instancedGameObjects;
	std::vector<CGameObject*> instancedGameObjectsWithAlpha;
	std::vector<int> indicesOfOutlineModels;
	std::vector<LightPair> pointlights;
	std::vector<LightPair> pointLightsInstanced;

#ifdef USING_DEFERRED // Define found under #includes
#pragma region Deferred
	for (unsigned int i = 0; i < gameObjects.size(); ++i)
	{
		auto instance = gameObjects[i];
		for (auto gameObjectToOutline : aScene.GetModelsToOutline()) {
			if (instance == gameObjectToOutline) {
				indicesOfOutlineModels.emplace_back(i);
			}
		}

		if (instance->GetComponent<CInstancedModelComponent>()) {
			if (instance->GetComponent<CInstancedModelComponent>()->RenderWithAlpha())
			{
				instancedGameObjectsWithAlpha.emplace_back(instance);
				continue;
			}
			instancedGameObjects.emplace_back(instance);
		}
	}

	std::sort(indicesOfOutlineModels.begin(), indicesOfOutlineModels.end(), [](UINT a, UINT b) { return a > b; });

	for (auto index : indicesOfOutlineModels)
	{
		std::swap(gameObjects[index], gameObjects.back());
		gameObjects.pop_back();
	}

	std::vector<CPointLight*> onlyPointLights;
	onlyPointLights = aScene.CullPointLights(&maincamera->GameObject());

	myGBuffer.SetAsActiveTarget(&myIntermediateDepth);
	myDeferredRenderer.GenerateGBuffer(maincamera, gameObjects, instancedGameObjects);
	myDeferredTexture.SetAsActiveTarget();
	myGBuffer.SetAllAsResources();
	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_ADDITIVEBLEND);

	if (myUseBloom)// Rename myUseBloom to be tied to renderpasses
	{
		myDeferredRenderer.Render(maincamera, environmentlight);
		myDeferredRenderer.Render(maincamera, onlyPointLights);
	}

	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_DISABLE);
	myIntermediateTexture.SetAsActiveTarget();
	myDeferredTexture.SetAsResourceOnSlot(0);
	if(myUseBloom)
		myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCRENSHADER_GAMMACORRECTION);

	myIntermediateTexture.SetAsActiveTarget(&myIntermediateDepth);

#pragma endregion ! Deferred
#else

	myIntermediateTexture.SetAsActiveTarget(&myIntermediateDepth);

	for (unsigned int i = 0; i < gameObjects.size(); ++i)
	{
		auto instance = gameObjects[i];
		for (auto gameObjectToOutline : aScene.GetModelsToOutline()) {
			if (instance == gameObjectToOutline) {
				indicesOfOutlineModels.emplace_back(i);
			}
		}

		if (instance->GetComponent<CModelComponent>()) {
			pointlights.emplace_back(aScene.CullLights(instance));
		}
		else if (instance->GetComponent<CInstancedModelComponent>()) {

			pointLightsInstanced.emplace_back(aScene.CullLightInstanced(instance->GetComponent<CInstancedModelComponent>()));

			if (instance->GetComponent<CInstancedModelComponent>()->RenderWithAlpha())
			{
				instancedGameObjectsWithAlpha.emplace_back(instance);
				continue;
			}
			instancedGameObjects.emplace_back(instance);
		}
	}

	std::sort(indicesOfOutlineModels.begin(), indicesOfOutlineModels.end(), [](UINT a, UINT b) { return a > b; });

	for (auto index : indicesOfOutlineModels)
	{
		std::swap(gameObjects[index], gameObjects.back());
		gameObjects.pop_back();
	}

	myForwardRenderer.Render(environmentlight, pointlights, maincamera, gameObjects);
	myForwardRenderer.InstancedRender(environmentlight, pointLightsInstanced, maincamera, instancedGameObjects);

#endif // USING_DEFERRED

#pragma region MODEL OUTLINES
	for (auto modelToOutline : aScene.GetModelsToOutline()) {
		std::vector<CGameObject*> interimVector;
		if (modelToOutline) {
			pointlights.emplace_back(aScene.CullLights(modelToOutline));
			interimVector.emplace_back(modelToOutline);
			myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_STENCILWRITE, 0xFF);

			myForwardRenderer.Render(environmentlight, pointlights, maincamera, interimVector);

			if (modelToOutline != aScene.GetPlayer()) {
				modelToOutline->GetComponent<CTransformComponent>()->SetOutlineScale();
			}

			myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_STENCILMASK, 0xFF);

			if (modelToOutline != aScene.GetPlayer()) {
				myForwardRenderer.RenderOutline(maincamera, modelToOutline, CModelFactory::GetInstance()->GetOutlineModelSubset(), { 1.0f, 0.0f, 0.0f, 1.0f });
				modelToOutline->GetComponent<CTransformComponent>()->ResetScale();
			}
			else {
				myForwardRenderer.RenderOutline(maincamera, modelToOutline, CModelFactory::GetInstance()->GetOutlineModelSubset(), { 25.0f / 255.0f, 200.0f / 255.0f, 208.0f / 255.0f, 1.0f });
			}
		}
	}
#pragma endregion ! MODEL OUTLINES

	const std::vector<CLineInstance*>& lineInstances = aScene.CullLineInstances();
	const std::vector<SLineTime>& lines = aScene.CullLines();
	myForwardRenderer.RenderLines(maincamera, lines);
	myForwardRenderer.RenderLineInstances(maincamera, lineInstances);

	// Alpha stage for objects in World 3D space
	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_ALPHABLEND);
	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_ONLYREAD);

	myForwardRenderer.InstancedRender(environmentlight, pointlights, maincamera, instancedGameObjectsWithAlpha);

	myVFXRenderer.Render(maincamera, gameObjects);

	myParticleRenderer.Render(maincamera, gameObjects);
	// ! Alpha stage for objects in World 3D space

	//std::vector<CSpriteInstance*> sprites = myScene.CullSprites();
	//mySpriteRenderer.Render(sprites);

	//std::vector<CAnimatedUIElement*> animatedUIElements = myScene.CullAnimatedUI();
	//mySpriteRenderer.Render(animatedUIElements);

	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_DISABLE);
	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_DEFAULT);

	// Hope this works!
	myUseBloom ? RenderBloom() : RenderWithoutBloom();

	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_ALPHABLEND);
	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_ONLYREAD);

	std::vector<CSpriteInstance*> sprites = aScene.CullSprites();
	CMainSingleton::PopupTextService().EmplaceSprites(sprites);
	CMainSingleton::DialogueSystem().EmplaceSprites(sprites);
	mySpriteRenderer.Render(sprites);

	std::vector<CSpriteInstance*> animatedUIFrames;
	std::vector<CAnimatedUIElement*> animatedUIElements = aScene.CullAnimatedUI(animatedUIFrames);
	CEngine::GetInstance()->GetActiveScene().GetMainCamera()->EmplaceSprites(animatedUIFrames);
	mySpriteRenderer.Render(animatedUIElements);
	mySpriteRenderer.Render(animatedUIFrames);

	myRenderStateManager.SetBlendState(CRenderStateManager::BlendStates::BLENDSTATE_DISABLE);
	myRenderStateManager.SetDepthStencilState(CRenderStateManager::DepthStencilStates::DEPTHSTENCILSTATE_DEFAULT);

	std::vector<CTextInstance*> textsToRender = aScene.GetTexts();
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
	myQuaterSizeTexture.ReleaseTexture();
	myBlurTexture1.ReleaseTexture();
	myBlurTexture2.ReleaseTexture();
	myVignetteTexture.ReleaseTexture();
	myDeferredTexture.ReleaseTexture();
	//myGBuffer // Should something be released for the GBuffer?
}

void CRenderManager::Clear(DirectX::SimpleMath::Vector4 aClearColor)
{
	myBackbuffer.ClearTexture(aClearColor);
	myIntermediateDepth.ClearDepth();
}

void CRenderManager::RenderBloom()
{
	myLuminanceTexture.SetAsActiveTarget();
	myIntermediateTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_LUMINANCE);

	myHalfSizeTexture.SetAsActiveTarget();
	myLuminanceTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPY);

	myQuaterSizeTexture.SetAsActiveTarget();
	myHalfSizeTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPY);

	myBlurTexture1.SetAsActiveTarget();
	myQuaterSizeTexture.SetAsResourceOnSlot(0);
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

	myQuaterSizeTexture.SetAsActiveTarget();
	myBlurTexture1.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPY);

	myHalfSizeTexture.SetAsActiveTarget();
	myQuaterSizeTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_COPY);

	myVignetteTexture.SetAsActiveTarget();
	myIntermediateTexture.SetAsResourceOnSlot(0);
	myFullscreenRenderer.Render(CFullscreenRenderer::FullscreenShader::FULLSCREENSHADER_VIGNETTE);

	myBackbuffer.SetAsActiveTarget();
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
