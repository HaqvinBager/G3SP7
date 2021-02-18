#include "stdafx.h"
#include "DecalRenderer.h"
#include "RenderManager.h"
#include "CameraComponent.h"
#include "DecalComponent.h"
#include "Decal.h"

#include <fstream>

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

void CDecalRenderer::Render(CCameraComponent* aCamera, std::vector<CGameObject*>& aGameObjectList)
{
	SM::Matrix& cameraMatrix = aCamera->GameObject().myTransform->Transform();
	myFrameBufferData.myCameraPosition = SM::Vector4{ cameraMatrix._41, cameraMatrix._42, cameraMatrix._43, 1.f };
	myFrameBufferData.myToCameraSpace = cameraMatrix.Invert();
	myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();

	BindBuffer(myFrameBuffer, myFrameBufferData, "Frame Buffer");

	myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
	myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);

	for (auto& gameObject : aGameObjectList)
	{
		CDecalComponent* decalComponent = gameObject->GetComponent<CDecalComponent>();
		if (decalComponent == nullptr)
			continue;

		if (decalComponent->GetMyDecal() == nullptr)
			continue;

		CDecal* decal = decalComponent->GetMyDecal();
		CDecal::SDecalData decalData = decal->GetDecalData();

		myObjectBufferData.myToWorld = gameObject->myTransform->GetWorldMatrix();
		BindBuffer(myObjectBuffer, myObjectBufferData, "Object Buffer");

		myContext->IASetPrimitiveTopology(decalData.myPrimitiveTopology);
		myContext->IASetInputLayout(decalData.myInputLayout);

		myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
		myContext->VSSetShader(decalData.myVertexShader, nullptr, 0);

		myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);

		myContext->PSSetShader(myPixelShader, nullptr, 0);
		myContext->PSSetSamplers(0, 1, &decalData.mySamplerState);

		myContext->IASetVertexBuffers(0, 1, &decalData.myMesh.myVertexBuffer, &decalData.myMesh.myStride, &decalData.myMesh.myOffset);
		myContext->IASetIndexBuffer(decalData.myMesh.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		myContext->PSSetShaderResources(5, 3, &decalData.myMaterials[decalData.myMesh.myMaterialIndex][0]);
		myContext->DrawIndexed(decalData.myMesh.myNumberOfIndices, 0, 0);
		CRenderManager::myNumberOfDrawCallsThisFrame++;
	}
}

bool CDecalRenderer::CreatePixelShader(std::string aFilepath, CDirectXFramework* aFramework, ID3D11PixelShader** outPixelShader)
{
	std::ifstream psFile;
	psFile.open(aFilepath, std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, outPixelShader), "Pixel Shader could not be created.");
	psFile.close();
	return true;
}
