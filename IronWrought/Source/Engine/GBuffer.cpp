#include "stdafx.h"
#include "GBuffer.h"
#include "FullscreenTexture.h"

CGBuffer::CGBuffer()
{
}

CGBuffer::~CGBuffer()
{
}

void CGBuffer::ClearTextures(DirectX::SimpleMath::Vector4 aClearColor)
{
	for (UINT i = 0; i < static_cast<size_t>(EGBufferTextures::COUNT); ++i) {
		myContext->ClearRenderTargetView(myRenderTargets[i], &aClearColor.x);
	}
}

void CGBuffer::ReleaseRenderTargets()
{
	std::array<ID3D11RenderTargetView*, static_cast<size_t>(EGBufferTextures::COUNT)> nullViews = { NULL, NULL, NULL, NULL/*, NULL, NULL, NULL, NULL*/ };
	myContext->OMSetRenderTargets(static_cast<size_t>(EGBufferTextures::COUNT), &nullViews[0], nullptr);
}

void CGBuffer::SetAsActiveTarget(CFullscreenTexture* aDepth)
{
	if (aDepth)
	{
		myContext->OMSetRenderTargets(static_cast<size_t>(EGBufferTextures::COUNT), &myRenderTargets[0], aDepth->myDepth);
	}
	else
	{
		myContext->OMSetRenderTargets(static_cast<size_t>(EGBufferTextures::COUNT), &myRenderTargets[0], nullptr);
	}
	myContext->RSSetViewports(1, myViewport);
}

void CGBuffer::SetAsResourceOnSlot(EGBufferTextures aResource, UINT aSlot)
{
	myContext->PSSetShaderResources(aSlot, 1, &myShaderResources[static_cast<size_t>(aResource)]);
}

void CGBuffer::SetAllAsResources()
{
	myContext->PSSetShaderResources(1, static_cast<size_t>(EGBufferTextures::COUNT), &myShaderResources[0]);
}
