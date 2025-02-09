#include "stdafx.h"
#include "ForwardRenderer.h"
#include "Engine.h"
#include "RenderManager.h"
#include "Model.h"
#include "Camera.h"
#include "EnvironmentLight.h"
#include "ModelMath.h"
#include "GameObject.h"
#include "ModelComponent.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"
#include "PointLight.h"
#include "Line.h"
#include "LineInstance.h"
#include "Debug.h"
#include "CameraComponent.h"
#include "InstancedModelComponent.h"
#include "Scene.h"

#include <fstream>

namespace SM = DirectX::SimpleMath;

CForwardRenderer::CForwardRenderer()
	: myContext(nullptr)
	, myFrameBuffer(nullptr)
	, myFrameBufferData()
	, myObjectBuffer(nullptr)
	, myObjectBufferData()
	, myBoneBuffer(nullptr)
	, myBoneBufferData()
	, myOutlineBuffer(nullptr)
	, myOutlineBufferData()
	, myRenderPassIndex(100)
{

}

CForwardRenderer::~CForwardRenderer() {
	myContext = nullptr;
}

bool CForwardRenderer::Init(CDirectXFramework* aFramework) {
	if (!aFramework)
		return false;

	myContext = aFramework->GetContext();
	if (!myContext) {
		return false;
	}

	ID3D11Device* device = aFramework->GetDevice();
	if (!device)
		return false;

	D3D11_BUFFER_DESC bufferDescription = {0};
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.MiscFlags = 0;
	bufferDescription.ByteWidth = static_cast<UINT>(sizeof(SFrameBufferData) + (16 - (sizeof(SFrameBufferData) % 16)));
	bufferDescription.StructureByteStride = 0;

	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer), "Frame Buffer could not be created.");

	bufferDescription.ByteWidth = static_cast<UINT>(sizeof(SObjectBufferData) + (16 - (sizeof(SObjectBufferData) % 16)));
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer), "Object Buffer could not be created.");

	bufferDescription.ByteWidth = static_cast<UINT>(sizeof(SBoneBufferData) + (16 - (sizeof(SBoneBufferData) % 16)));
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myBoneBuffer), "Bone Buffer could not be created.");

	bufferDescription.ByteWidth = static_cast<UINT>(sizeof(SOutlineBufferData) + (16 - (sizeof(SOutlineBufferData) % 16)));
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myOutlineBuffer), "Outline Buffer could not be created.");

	// Render pass shaders
	std::ifstream psFile;
	psFile.open("Shaders/RenderPassColorPixelShader.cso", std::ios::binary);
	std::string psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();

	myRenderPassPixelShaders.emplace_back();
	ENGINE_HR_MESSAGE(device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassPixelShaders[0]), "Color Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/RenderPassNormalPixelShader.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();

	myRenderPassPixelShaders.emplace_back();
	ENGINE_HR_MESSAGE(device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassPixelShaders[1]), "Normal Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/RenderPassRoughnessPixelShader.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();

	myRenderPassPixelShaders.emplace_back();
	ENGINE_HR_MESSAGE(device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassPixelShaders[2]), "Roughness Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/RenderPassMetalnessPixelShader.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();

	myRenderPassPixelShaders.emplace_back();
	ENGINE_HR_MESSAGE(device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassPixelShaders[3]), "Metalness Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/RenderPassAmbientOcclusionPixelShader.cso", std::ios::binary);
	psData = {std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>()};
	psFile.close();

	myRenderPassPixelShaders.emplace_back();
	ENGINE_HR_MESSAGE(device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassPixelShaders[4]), "Ambient Occlusion Pixel Shader could not be created.");

	// ===============
	psFile.open("Shaders/RenderPassEmissivePixelShader.cso", std::ios::binary);
	psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	psFile.close();

	myRenderPassPixelShaders.emplace_back();
	ENGINE_HR_MESSAGE(device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myRenderPassPixelShaders[5]), "Ambient Occlusion Pixel Shader could not be created.");

	return true;
}

void CForwardRenderer::Render(CEnvironmentLight* anEnvironmentLight, std::vector<std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>>> aModelPointLightList, CCameraComponent* aCamera, std::vector<CGameObject*>& aGameObjectList)
{
	DirectX::SimpleMath::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myCameraPosition = DirectX::SimpleMath::Vector4{cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f};
	myFrameBufferData.myDirectionalLightDirection = anEnvironmentLight->GetDirection();
	myFrameBufferData.myDirectionalLightColor = anEnvironmentLight->GetColor();

	myFrameBufferData.myDirectionalLightView = anEnvironmentLight->GetShadowView();
	myFrameBufferData.myDirectionalLightProjection = anEnvironmentLight->GetShadowProjection();
	myFrameBufferData.myDirectionalLightPosition = anEnvironmentLight->GetShadowPosition();
	myFrameBufferData.myDirectionalLightShadowMapResolution = anEnvironmentLight->GetShadowmapResolution();

	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetShaderResources(0, 1, anEnvironmentLight->GetCubeMap());

	//MODELCOMPONENT
	int modelIndex = 0;
	for (auto& gameobject : aGameObjectList)
	{
		//Added this if Check because not all GameObjects Must have a CModelComponent.
		//Refactoring suggestion: Have CModelComponents be "created" in some kind of Factory.
		//This factory will make sure to hold all CModelComponent Data in a Cache friendly array <3
		//Big hype!

		if (gameobject->GetComponent<CModelComponent>() == nullptr)
			continue;

		if (gameobject->GetComponent<CModelComponent>()->GetMyModel() == nullptr)
			continue;

		for (unsigned int i = 0; i < aModelPointLightList[modelIndex].first; ++i)
		{
			const SM::Vector3& position = aModelPointLightList[modelIndex].second[i]->GetPosition();
			const SM::Vector3& color = aModelPointLightList[modelIndex].second[i]->GetColor();
			myObjectBufferData.myPointLights[i].myPositionAndIntensity = {position.x, position.y, position.z, aModelPointLightList[modelIndex].second[i]->GetIntensity()};
			myObjectBufferData.myPointLights[i].myColorAndRange = {color.x, color.y, color.z, aModelPointLightList[modelIndex].second[i]->GetRange()};
		}
		myObjectBufferData.myNumberOfUsedPointLights = aModelPointLightList[modelIndex].first;
		modelIndex++;

		CModel* model = gameobject->GetComponent<CModelComponent>()->GetMyModel();

		const CModel::SModelData& modelData = model->GetModelData();

		myObjectBufferData.myToWorld = gameobject->myTransform->Transform();

		int counter = 0;
		for (auto detailNormal : model->GetModelData().myDetailNormals)
		{
			if (detailNormal)
			{
				++counter;
			}
		}
		myObjectBufferData.myNumberOfDetailNormals = counter;

		BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

		/* 2021 02 09 (Minor issue)
		* If a model has AnimatedVertexShader but no CAnimationComponent it will use the bone data from the last bound BoneData. Meaning it will mimic another 
		* GameObjects animations.
		* Since a model gets its vertexshader in ModelFactory based on if it has bones in its mesh it is hard to avoid identical models getting the same type of vertex shader.
		* Sort of a non-issue since we are required to use deferred rendering (and all animated models should be passed through deferred).
		*/
		if (gameobject->GetComponent<CAnimationComponent>() != nullptr) {
			memcpy(myBoneBufferData.myBones, gameobject->GetComponent<CAnimationComponent>()->GetBones().data(), sizeof(SlimMatrix44) * 64);

			BindBuffer(myBoneBuffer, myBoneBufferData, "Bone Buffer");

			myContext->VSSetConstantBuffers(2, 1, &myBoneBuffer);
		}

		myContext->IASetPrimitiveTopology(modelData.myPrimitiveTopology);
		myContext->IASetInputLayout(modelData.myInputLayout);

		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(modelData.myVertexShader, nullptr, 0);

		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		for (unsigned int i = 0; i < myObjectBufferData.myNumberOfDetailNormals; ++i)
		{
			myContext->PSSetShaderResources(4 + i, 1, &modelData.myDetailNormals[i]);
		}
		myContext->PSSetSamplers(0, 1, &modelData.mySamplerState);

		// Toggling render passes
		if (myCurrentPixelShader == nullptr)
			myContext->PSSetShader(modelData.myPixelShader, nullptr, 0);
		else
			myContext->PSSetShader(myCurrentPixelShader, nullptr, 0);

		// Render all meshes
		for (unsigned int i = 0; i < modelData.myMeshes.size(); ++i)
		{
			myContext->IASetVertexBuffers(0, 1, &modelData.myMeshes[i].myVertexBuffer, &modelData.myMeshes[i].myStride, &modelData.myMeshes[i].myOffset);
			myContext->IASetIndexBuffer(modelData.myMeshes[i].myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(1, 3, &modelData.myMaterials[modelData.myMeshes[i].myMaterialIndex][0]);
			myContext->DrawIndexed(modelData.myMeshes[i].myNumberOfIndices, 0, 0);
			CRenderManager::myNumberOfDrawCallsThisFrame++;
		}
	}
}

void CForwardRenderer::InstancedRender(CEnvironmentLight* anEnvironmentLight, std::vector<std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>>> aModelPointLightList, CCameraComponent* aCamera, std::vector<CGameObject*>& aGameObjectList)
{
	DirectX::SimpleMath::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
	myFrameBufferData.myCameraPosition = DirectX::SimpleMath::Vector4{cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f};
	myFrameBufferData.myDirectionalLightDirection = anEnvironmentLight->GetDirection();
	myFrameBufferData.myDirectionalLightColor = anEnvironmentLight->GetColor();

	myFrameBufferData.myDirectionalLightView = anEnvironmentLight->GetShadowView();
	myFrameBufferData.myDirectionalLightProjection = anEnvironmentLight->GetShadowProjection();
	myFrameBufferData.myDirectionalLightPosition = anEnvironmentLight->GetShadowPosition();
	myFrameBufferData.myDirectionalLightShadowMapResolution = anEnvironmentLight->GetShadowmapResolution();

	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
	//ID3D11ShaderResourceView* nullView = NULL;
	//myContext->PSSetShaderResources(0, 1, &nullView);
	myContext->PSSetShaderResources(0, 1, anEnvironmentLight->GetCubeMap());

	// MODELCOMPONENT
	for (auto& gameobject : aGameObjectList)
	{
		CInstancedModelComponent* instanceComponent = gameobject->GetComponent<CInstancedModelComponent>();
		if (instanceComponent == nullptr)
			continue;


		//InstancedRender(aScene, instanceComponent);

		/*const std::vector<Matrix>& instanceMatrixes = instanceComponent->GetInstancedTransforms();
		for (unsigned int instanceIndex = 0; instanceIndex < instanceMatrixes.size(); ++instanceIndex)
		{
			for (unsigned int pointLightCount = 0; pointLightCount < aModelPointLightList[instanceIndex].first; ++pointLightCount)
			{
				SM::Vector3 lightPosition = aModelPointLightList[gameObjectIndex].second[pointLightCount]->GetPosition();
				SM::Vector3 lightColor = aModelPointLightList[gameObjectIndex].second[pointLightCount]->GetColor();

				float lightRange = aModelPointLightList[gameObjectIndex].second[pointLightCount]->GetRange();
			}
		}*/

		for (unsigned int matrix = 0; matrix < instanceComponent->GetInstancedTransforms().size(); ++matrix) {
			int instancedIndex = 0;
			for (unsigned int i = 0; i < aModelPointLightList[instancedIndex].first; ++i) {
				SM::Vector3 position = aModelPointLightList[instancedIndex].second[i]->GetPosition();
				SM::Vector3 color = aModelPointLightList[instancedIndex].second[i]->GetColor();
				myObjectBufferData.myPointLights[i].myPositionAndIntensity = {position.x, position.y, position.z, aModelPointLightList[instancedIndex].second[i]->GetIntensity()};
				myObjectBufferData.myPointLights[i].myColorAndRange = {color.x, color.y, color.z, aModelPointLightList[instancedIndex].second[i]->GetRange()};
			}
			myObjectBufferData.myNumberOfUsedPointLights = aModelPointLightList[instancedIndex].first;
			instancedIndex++;

			BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");
		}

		CModel* model = instanceComponent->GetModel();
		const CModel::SModelInstanceData& modelData = model->GetModelInstanceData();

		{
			D3D11_MAPPED_SUBRESOURCE instanceBuffer;
			ZeroMemory(&instanceBuffer, sizeof(D3D11_MAPPED_SUBRESOURCE));
			ENGINE_HR_MESSAGE(myContext->Map(modelData.myInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceBuffer), "Instanced Buffer Could not be mapped in ForwardRenderer.");
			memcpy(instanceBuffer.pData, &instanceComponent->GetInstancedTransforms()[0], sizeof(DirectX::SimpleMath::Matrix) * instanceComponent->GetInstancedTransforms().size());
			myContext->Unmap(modelData.myInstanceBuffer, 0);
		}

		myContext->IASetPrimitiveTopology(modelData.myPrimitiveTopology);
		myContext->IASetInputLayout(modelData.myInputLayout);

		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(modelData.myVertexShader, nullptr, 0);

		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->PSSetSamplers(0, 1, &modelData.mySamplerState);

		// Toggling render passes
		if (myCurrentPixelShader == nullptr)
			myContext->PSSetShader(modelData.myPixelShader, nullptr, 0);
		else
			myContext->PSSetShader(myCurrentPixelShader, nullptr, 0);

		// Render all meshes
		for (unsigned int i = 0; i < modelData.myMeshes.size(); ++i)
		{
			ID3D11Buffer* bufferPointers[2] = { modelData.myMeshes[i].myVertexBuffer, modelData.myInstanceBuffer };
			myContext->IASetVertexBuffers(0, 2, bufferPointers, modelData.myMeshes[i].myStride, modelData.myMeshes[i].myOffset);
			myContext->IASetIndexBuffer(modelData.myMeshes[i].myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(1, 3, &modelData.myMaterials[modelData.myMeshes[i].myMaterialIndex][0]);
			myContext->DrawIndexedInstanced(modelData.myMeshes[i].myNumberOfIndices, model->InstanceCount(), 0, 0, 0);
			CRenderManager::myNumberOfDrawCallsThisFrame++;
		}
	}
}

void CForwardRenderer::RenderLines(CCameraComponent* aCamera, const std::vector<SLineTime>& aLineList) {

	namespace SM = DirectX::SimpleMath;
	myFrameBufferData.myToCameraSpace = aCamera->GameObject().myTransform->Transform().Invert();
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();

	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	for (const SLineTime& instance : aLineList)
	{
		const CLine::SLineData& lineData = instance.myLine.GetLine()->GetLineData();

		myObjectBufferData.myToWorld = instance.myLine.GetTransform();

		BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

		myContext->IASetPrimitiveTopology(lineData.myPrimitiveTopology);
		myContext->IASetInputLayout(lineData.myInputLayout);
		myContext->IASetVertexBuffers(0, 1, &lineData.myVertexBuffer, &lineData.myStride, &lineData.myOffset);
		myContext->IASetIndexBuffer(lineData.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(lineData.myVertexShader, nullptr, 0);

		myContext->PSSetShader(lineData.myPixelShader, nullptr, 0);

		//myContext->DrawIndexed(lineData.myNumberOfIndices, 0, 0);
		myContext->Draw(lineData.myNumberOfVertices, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}
}

void CForwardRenderer::RenderOutline(CCameraComponent* aCamera, CGameObject* aModelInstance, CModel* someOutlineModelData, DirectX::SimpleMath::Vector4 aColor)
{
	if (!someOutlineModelData) {
		return;
	}
	DirectX::SimpleMath::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = DirectX::SimpleMath::Vector4(cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f);
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();

	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	CModel* model = aModelInstance->GetComponent<CModelComponent>()->GetMyModel();
	CModel::SModelData modelData = model->GetModelData();
	CModel::SModelData outlineModelData = someOutlineModelData->GetModelData();

	myObjectBufferData.myToWorld = aModelInstance->GetComponent<CTransformComponent>()->Transform();

	BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

	myContext->IASetPrimitiveTopology(modelData.myPrimitiveTopology);
	myContext->IASetInputLayout(modelData.myInputLayout);

	//myContext->IASetVertexBuffers(0, 1, &modelData.myVertexBuffer, &modelData.myStride, &modelData.myOffset);
	//myContext->IASetIndexBuffer(modelData.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
	myContext->VSSetShader(outlineModelData.myVertexShader, nullptr, 0);

	myOutlineBufferData.myOutlineColor = aColor;
	BindBuffer(myOutlineBuffer, myOutlineBufferData, "Outline Buffer");

	myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
	myContext->PSSetConstantBuffers(3, 1, &myOutlineBuffer);
	//myContext->PSSetShaderResources(1, 3, &modelData.myTexture[0]);
	myContext->PSSetShader(outlineModelData.myPixelShader, nullptr, 0);

	myContext->PSSetSamplers(0, 1, &modelData.mySamplerState);

	for (unsigned int i = 0; i < modelData.myMeshes.size(); ++i)
	{
		myContext->IASetVertexBuffers(0, 1, &modelData.myMeshes[i].myVertexBuffer, &modelData.myMeshes[i].myStride, &modelData.myMeshes[i].myOffset);
		myContext->IASetIndexBuffer(modelData.myMeshes[i].myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//myContext->PSSetShaderResources(1, 3, &modelData.myMaterials[modelData.myMeshes[i].myMaterialIndex][0]);
		myContext->DrawIndexed(modelData.myMeshes[i].myNumberOfIndices, 0, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}

	//myContext->DrawIndexed(modelData.myNumberOfIndices, 0, 0);
}

void CForwardRenderer::RenderLineInstances(CCameraComponent* aCamera, const std::vector<CLineInstance*>& aLineList) {

	namespace SM = DirectX::SimpleMath;
	myFrameBufferData.myToCameraSpace = aCamera->GameObject().myTransform->Transform().Invert();
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();

	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	for (const CLineInstance* instance : aLineList)
	{
		const CLine::SLineData& lineData = instance->GetLine()->GetLineData();

		myObjectBufferData.myToWorld = instance->GetTransform();

		BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

		myContext->IASetPrimitiveTopology(lineData.myPrimitiveTopology);
		myContext->IASetInputLayout(lineData.myInputLayout);
		myContext->IASetVertexBuffers(0, 1, &lineData.myVertexBuffer, &lineData.myStride, &lineData.myOffset);
		myContext->IASetIndexBuffer(lineData.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(lineData.myVertexShader, nullptr, 0);

		myContext->PSSetShader(lineData.myPixelShader, nullptr, 0);

		//myContext->DrawIndexed(lineData.myNumberOfIndices, 0, 0);
		myContext->Draw(lineData.myNumberOfVertices, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}
}

bool CForwardRenderer::ToggleRenderPass(bool aShouldToggleForwards)
{
	if (!aShouldToggleForwards)
		--myRenderPassIndex; 
	else 
		++myRenderPassIndex;


	if (myRenderPassIndex < 0)
	{
		myRenderPassIndex = static_cast<int>(myRenderPassPixelShaders.size());
	}
	if (myRenderPassIndex == myRenderPassPixelShaders.size())
	{
		myCurrentPixelShader = nullptr;
		return true;
	}
	else if (myRenderPassIndex > myRenderPassPixelShaders.size())
	{
		myRenderPassIndex = 0;
	}
	myCurrentPixelShader = myRenderPassPixelShaders[myRenderPassIndex];
	return false;
}

bool CForwardRenderer::ToggleRenderPass(int anIndex)
{
	myRenderPassIndex = anIndex - 1;

	if (myRenderPassIndex >= myRenderPassPixelShaders.size() || myRenderPassIndex < 0)
	{
		myCurrentPixelShader = nullptr;
		return true;
	}

	myCurrentPixelShader = myRenderPassPixelShaders[myRenderPassIndex];
	return false;
}
