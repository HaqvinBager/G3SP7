#include "stdafx.h"
#include "ParticleRenderer.h"
#include "DirectXFramework.h"
#include "RenderManager.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include <iostream>

#include "VFXSystemComponent.h"
#include "VFXEffect.h"

CParticleRenderer::CParticleRenderer() : myContext(nullptr), myFrameBuffer(), myObjectBuffer()
{
}

CParticleRenderer::~CParticleRenderer()
{
}

bool CParticleRenderer::Init(CDirectXFramework* aFramework)
{
    myContext = aFramework->GetContext();
    if (!myContext) {
        return false;
    }

    ID3D11Device* device = aFramework->GetDevice();
    if (!device) {
        return false;
    }

    HRESULT result;
    D3D11_BUFFER_DESC bufferDescription = { 0 };
    bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
    bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    bufferDescription.ByteWidth = sizeof(SFrameBufferData);
    result = device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
    if (FAILED(result))
    {
        return false;
    }

    bufferDescription.ByteWidth = sizeof(SObjectBufferData);
    result = device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void CParticleRenderer::Render(CCameraComponent* aCamera, std::vector<CGameObject*>& aGameObjectList)
{
    D3D11_MAPPED_SUBRESOURCE bufferData;

    myFrameBufferData.myToCameraSpace = aCamera->GameObject().myTransform->Transform().Invert();
    myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();

    BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

    myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
    myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);

    for (CGameObject* gameObject : aGameObjectList)
    {
        CVFXSystemComponent* component = gameObject->GetComponent<CVFXSystemComponent>();
        if (component == nullptr)
            continue;

        auto& effects = component->GetVFXEffects();
        for (unsigned int j = 0; j < effects.size(); ++j)
        {
            auto& effect = effects[j];

            const std::vector<CParticleEmitter*>& particles = effect->GetParticleSet();
            if (particles.empty())
                continue;

            const std::vector<Matrix>& transforms = effect->GetParticleTransforms();

            for (unsigned int i = 0; i < particles.size(); ++i) {
                if (effect->GetParticleVertices()[i].size() < 1) {
                    continue;
                }

                myObjectBufferData.myToWorld = transforms[i];
                BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

                const CParticleEmitter::SParticleData& particleData = particles[i]->GetParticleData();

                ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
                ENGINE_HR_MESSAGE(myContext->Map(particleData.myParticleVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData), "Vertex Buffer could not be mapped.");

                UINT numberOfVertices = (UINT)effect->GetParticleVertices()[i].size();

                memcpy(bufferData.pData, &(effect->GetParticleVertices()[i][0]), sizeof(CParticleEmitter::SParticleVertex) * numberOfVertices);
                myContext->Unmap(particleData.myParticleVertexBuffer, 0);

                myContext->IASetPrimitiveTopology(particleData.myPrimitiveTopology);
                myContext->IASetInputLayout(particleData.myInputLayout);
                myContext->IASetVertexBuffers(0, 1, &particleData.myParticleVertexBuffer, &particleData.myStride, &particleData.myOffset);

                myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
                myContext->VSSetShader(particleData.myVertexShader, nullptr, 0);

                myContext->GSSetShader(particleData.myGeometryShader, nullptr, 0);

                myContext->PSSetSamplers(0, 1, &particleData.mySampler);
                myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
                myContext->PSSetShaderResources(0, 1, &particleData.myTexture);
                myContext->PSSetShader(particleData.myPixelShader, nullptr, 0);

                myContext->Draw(numberOfVertices, 0);
                CRenderManager::myNumberOfDrawCallsThisFrame++;
            }
        }

        //Reset Resources
        ID3D11ShaderResourceView* nullView = NULL;
        myContext->PSSetShaderResources(0, 1, &nullView);
        myContext->PSSetShaderResources(1, 1, &nullView);

        myContext->GSSetShader(nullptr, nullptr, 0);
        
    }
}
