#include "stdafx.h"
#include "ShadowRenderer.h"
#include "GameObject.h"
#include "EnvironmentLight.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "Model.h"
#include "ModelComponent.h"
#include "AnimationComponent.h"
#include "InstancedModelComponent.h"
#include "RenderManager.h"
#include "GraphicsHelpers.h"

CShadowRenderer::CShadowRenderer()
	: myContext(nullptr)
	, myShadowSampler(nullptr)
	, myFrameBuffer(nullptr)
	, myObjectBuffer(nullptr)
{
}

CShadowRenderer::~CShadowRenderer()
{
}

bool CShadowRenderer::Init(CDirectXFramework* aFramework)
{
	myContext = aFramework->GetContext();
	ENGINE_ERROR_BOOL_MESSAGE(myContext, "Context could not be acquired from Framework.");

	ID3D11Device* device = aFramework->GetDevice();
	ENGINE_ERROR_BOOL_MESSAGE(device, "Device could not be acquired from Framework.");

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(SFrameBufferData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer), "Frame Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SObjectBufferData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer), "Object Buffer could not be created.");

	bufferDescription.ByteWidth = static_cast<UINT>(sizeof(SBoneBufferData) + (16 - (sizeof(SBoneBufferData) % 16)));
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myBoneBuffer), "Bone Buffer could not be created.");

	std::string vsData = "";
	Graphics::CreateVertexShader("Shaders/DeferredModelVertexShader.cso", aFramework, &myModelVertexShader, vsData);
	Graphics::CreateVertexShader("Shaders/DeferredAnimationVertexShader.cso", aFramework, &myAnimationVertexShader, vsData);
	Graphics::CreateVertexShader("Shaders/DeferredInstancedModelVertexShader.cso", aFramework, &myInstancedModelVertexShader, vsData);

	return true;
}

void CShadowRenderer::Render(CEnvironmentLight* anEnvironmentLight, std::vector<CGameObject*>& aGameObjectList, std::vector<CGameObject*>& aInstancedGameObjectList)
{
	myFrameBufferData.myToCameraSpace = anEnvironmentLight->GetShadowTransform().Invert();
	SM::Vector3 pos;
	pos.x = anEnvironmentLight->GetShadowPosition().x;
	pos.y = anEnvironmentLight->GetShadowPosition().y;
	pos.z = anEnvironmentLight->GetShadowPosition().z;
	pos = SM::Vector3::Transform(pos, myFrameBufferData.myToCameraSpace);
	myFrameBufferData.myToCameraSpace(3, 0) = -pos.x;
	myFrameBufferData.myToCameraSpace(3, 1) = -pos.y;
	myFrameBufferData.myToCameraSpace(3, 2) = -pos.z;
	myFrameBufferData.myToProjectionSpace = anEnvironmentLight->GetShadowView();

	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");
	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);

	for (auto& gameObject : aGameObjectList)
	{
		CModelComponent* modelComponent = gameObject->GetComponent<CModelComponent>();
		if (modelComponent == nullptr)
			continue;

		if (modelComponent->GetMyModel() == nullptr)
			continue;

		CModel* model = modelComponent->GetMyModel();
		const CModel::SModelData& modelData = model->GetModelData();

		myObjectBufferData.myToWorld = gameObject->myTransform->Transform();
		BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

		if (gameObject->GetComponent<CAnimationComponent>() != nullptr) 
		{
			memcpy(myBoneBufferData.myBones, gameObject->GetComponent<CAnimationComponent>()->GetBones().data(), sizeof(Matrix) * 64);
			BindBuffer(myBoneBuffer, myBoneBufferData, "Bone Buffer");

			myContext->VSSetConstantBuffers(4, 1, &myBoneBuffer);
			myContext->VSSetShader(myAnimationVertexShader, nullptr, 0);
		}
		else
		{
			myContext->VSSetShader(myModelVertexShader, nullptr, 0);
		}

		myContext->IASetPrimitiveTopology(modelData.myPrimitiveTopology);
		myContext->IASetInputLayout(modelData.myInputLayout);
		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetShader(nullptr, nullptr, 0);

		// Render all meshes
		for (unsigned int i = 0; i < modelData.myMeshes.size(); ++i)
		{
			myContext->IASetVertexBuffers(0, 1, &modelData.myMeshes[i].myVertexBuffer, &modelData.myMeshes[i].myStride, &modelData.myMeshes[i].myOffset);
			myContext->IASetIndexBuffer(modelData.myMeshes[i].myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->DrawIndexed(modelData.myMeshes[i].myNumberOfIndices, 0, 0);
			CRenderManager::myNumberOfDrawCallsThisFrame++;
		}
	}

	for (auto& gameobject : aInstancedGameObjectList)
	{
		CInstancedModelComponent* instanceComponent = gameobject->GetComponent<CInstancedModelComponent>();
		if (instanceComponent == nullptr)
			continue;

		CModel* model = instanceComponent->GetModel();
		const CModel::SModelInstanceData& modelData = model->GetModelInstanceData();

		BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

		{
			D3D11_MAPPED_SUBRESOURCE instanceBuffer;
			ZeroMemory(&instanceBuffer, sizeof(D3D11_MAPPED_SUBRESOURCE));
			ENGINE_HR_MESSAGE(myContext->Map(modelData.myInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceBuffer), "Instanced Buffer Could not be mapped in Shadow Renderer.");
			memcpy(instanceBuffer.pData, &instanceComponent->GetInstancedTransforms()[0], sizeof(DirectX::SimpleMath::Matrix) * instanceComponent->GetInstancedTransforms().size());
			myContext->Unmap(modelData.myInstanceBuffer, 0);
		}

		myContext->IASetPrimitiveTopology(modelData.myPrimitiveTopology);
		myContext->IASetInputLayout(modelData.myInputLayout);
		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(myInstancedModelVertexShader, nullptr, 0);
		myContext->PSSetShader(nullptr, nullptr, 0);

		// Render all meshes
		for (unsigned int i = 0; i < modelData.myMeshes.size(); ++i)
		{
			ID3D11Buffer* bufferPointers[2] = { modelData.myMeshes[i].myVertexBuffer, modelData.myInstanceBuffer };
			myContext->IASetVertexBuffers(0, 2, bufferPointers, modelData.myMeshes[i].myStride, modelData.myMeshes[i].myOffset);
			myContext->IASetIndexBuffer(modelData.myMeshes[i].myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->DrawIndexedInstanced(modelData.myMeshes[i].myNumberOfIndices, model->InstanceCount(), 0, 0, 0);
			CRenderManager::myNumberOfDrawCallsThisFrame++;
		}
	}
}
