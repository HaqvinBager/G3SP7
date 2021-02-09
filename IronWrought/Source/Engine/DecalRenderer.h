#pragma once

class CEngine;
class CCameraComponent;
class CGameObject;

class CDecalRenderer
{
public:
	CDecalRenderer();
	~CDecalRenderer();

	bool Init(CDirectXFramework* aFramework);
	void Render(CCameraComponent* aCamera, std::vector<CGameObject*>& aGameObjectList);

private:
	template<class T>
	void BindBuffer(ID3D11Buffer* aBuffer, T& someBufferData, std::string aBufferType)
	{
		D3D11_MAPPED_SUBRESOURCE bufferData;
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		std::string errorMessage = aBufferType + " could not be bound.";
		ENGINE_HR_MESSAGE(myContext->Map(aBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData), errorMessage.c_str());

		memcpy(bufferData.pData, &someBufferData, sizeof(T));
		myContext->Unmap(aBuffer, 0);
	}

private:
	struct SFrameBufferData
	{
		DirectX::SimpleMath::Matrix myToCamera;
		DirectX::SimpleMath::Matrix myToProjection;
		DirectX::SimpleMath::Vector4 myCameraPosition;
	} myFrameBufferData;

	struct SObjectBufferData
	{
		DirectX::SimpleMath::Matrix myToWorld;
		unsigned int myNumberOfDetailNormals;
		unsigned int myNumberOfTextureSets;
		unsigned int myPadding[2];
	} myObjectBufferData;

private:
	ID3D11DeviceContext* myContext;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;
};

