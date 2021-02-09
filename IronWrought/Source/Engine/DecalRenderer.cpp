#include "stdafx.h"
#include "DecalRenderer.h"
#include "RenderManager.h"
#include "CameraComponent.h"

CDecalRenderer::CDecalRenderer()
	: myContext(nullptr)
	, myFrameBuffer(nullptr)
	, myObjectBuffer(nullptr)
{
}

CDecalRenderer::~CDecalRenderer()
{
}

bool CDecalRenderer::Init(CDirectXFramework* aFramework)
{
	myContext = aFramework->GetContext();
	ENGINE_ERROR_BOOL_MESSAGE(myContext, "Context could not be set.");

	ID3D11Device* device = aFramework->GetDevice();
	ENGINE_ERROR_BOOL_MESSAGE(device, "Device is nullptr");

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(SFrameBufferData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer), "Frame Buffer could not be created.");

	bufferDescription.ByteWidth = sizeof(SObjectBufferData);
	ENGINE_HR_BOOL_MESSAGE(device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer), "Object Buffer could not be created.");

	return true;
}

void CDecalRenderer::Render(CCameraComponent* /*aCamera*/, std::vector<CGameObject*>& /*aGameObjectList*/)
{
	//SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	//myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	//myFrameBufferData.myToCamera = cameraMatrix.Invert();
	//myFrameBufferData.myToProjection = aCamera->GetProjection();

	//BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	//myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	//myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	//for (auto& gameObject : aGameObjectList)
	//{
	//	CModelComponent* modelComponent = gameObject->GetComponent<CModelComponent>();
	//	if (modelComponent == nullptr)
	//		continue;

	//	if (modelComponent->GetMyModel() == nullptr)
	//		continue;

	//	CModel* model = modelComponent->GetMyModel();
	//	CModel::SModelData modelData = model->GetModelData();

	//	myObjectBufferData.myToWorld = gameObject->myTransform->GetWorldMatrix();
	//	int dnCounter = 0;
	//	for (auto detailNormal : model->GetModelData().myDetailNormals)
	//	{
	//		if (detailNormal)
	//			++dnCounter;
	//	}
	//	myObjectBufferData.myNumberOfDetailNormals = dnCounter;

	//	BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

	//	myContext->IASetPrimitiveTopology(modelData.myPrimitiveTopology);
	//	myContext->IASetInputLayout(modelData.myInputLayout);

	//	myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
	//	myContext->VSSetShader(myModelVertexShader, nullptr, 0);

	//	myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
	//	myContext->PSSetShaderResources(8, 4, &modelData.myDetailNormals[0]);

	//	myContext->PSSetShader(myCurrentGBufferPixelShader, nullptr, 0);
	//	myContext->PSSetSamplers(0, 1, &modelData.mySamplerState);

	//	// Vertex Paint
	//	unsigned int vertexColorID = modelComponent->VertexPaintColorID();

	//	// Render all meshes
	//	for (unsigned int i = 0; i < modelData.myMeshes.size(); ++i)
	//	{
	//		if (vertexColorID > 0)
	//		{
	//			auto vertexPaintMaterials = CMainSingleton::MaterialHandler().GetVertexPaintMaterials(modelComponent->VertexPaintMaterialNames());
	//			ID3D11Buffer* vertexColorBuffer = CMainSingleton::MaterialHandler().GetVertexColorBuffer(vertexColorID);
	//			UINT stride = sizeof(DirectX::SimpleMath::Vector3);
	//			ID3D11Buffer* bufferPointers[2] = { modelData.myMeshes[i].myVertexBuffer, vertexColorBuffer };
	//			UINT strides[2] = { modelData.myMeshes[i].myStride, stride };
	//			UINT offsets[2] = { 0, 0 };
	//			myContext->IASetInputLayout(myVertexPaintInputLayout);
	//			myContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	//			myContext->VSSetShader(myVertexPaintModelVertexShader, nullptr, 0);
	//			myContext->PSSetShader(myVertexPaintPixelShader, nullptr, 0);
	//			myContext->PSSetShaderResources(12, 9, &vertexPaintMaterials[0]/*&myVertexPaintMaterials[0]*/);
	//		}
	//		else {
	//			myContext->IASetVertexBuffers(0, 1, &modelData.myMeshes[i].myVertexBuffer, &modelData.myMeshes[i].myStride, &modelData.myMeshes[i].myOffset);
	//		}
	//		myContext->IASetIndexBuffer(modelData.myMeshes[i].myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//		myContext->PSSetShaderResources(5, 3, &modelData.myMaterials[modelData.myMeshes[i].myMaterialIndex][0]);
	//		myContext->DrawIndexed(modelData.myMeshes[i].myNumberOfIndices, 0, 0);
	//		CRenderManager::myNumberOfDrawCallsThisFrame++;
	//	}
	//}
}
