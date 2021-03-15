#pragma once

struct ID3D11DeviceContext;
struct ID3D11Buffer;
class CDirectXFramework;
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
		DirectX::SimpleMath::Matrix myToCameraSpace;
		DirectX::SimpleMath::Matrix myToProjectionSpace;
		DirectX::SimpleMath::Matrix myToWorldFromCamera;
		DirectX::SimpleMath::Matrix myToCameraFromProjection;
	} myFrameBufferData;

	struct SObjectBufferData
	{
		DirectX::SimpleMath::Matrix myToWorld;
		DirectX::SimpleMath::Matrix myToObjectSpace;
		float myAlphaClipThreshold;
		DirectX::SimpleMath::Vector3 myPadding;
	} myObjectBufferData;

private:
	ID3D11DeviceContext* myContext;

	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;

	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myIndexBuffer;
	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader* myPixelShader;

	ID3D11PixelShader* myAlbedoPixelShader;
	ID3D11PixelShader* myNormalPixelShader;
	ID3D11PixelShader* myMaterialPixelShader;

	ID3D11SamplerState* mySamplerState;
	ID3D11InputLayout* myInputLayout;
	D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology;
	UINT myNumberOfVertices;
	UINT myNumberOfIndices;
	UINT myStride;
	UINT myOffset;
};

