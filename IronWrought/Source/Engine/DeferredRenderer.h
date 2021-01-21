#pragma once
#include <vector>
#include "SimpleMath.h"

struct ID3D11DeviceContext;
struct ID3D11Buffer;
class CDirectXFramework;
class CModelInstance;
class CModel;
class CEnvironmentLight;
class CPointLight;
class CGameObject;
class CCameraComponent;

class CDeferredRenderer
{
public:
	CDeferredRenderer();
	~CDeferredRenderer();

	bool Init(CDirectXFramework* aFramework);

	void GenerateGBuffer(CCameraComponent* aCamera, std::vector<CGameObject*>& aGameObjectList, std::vector<CGameObject*>& aInstancedGameObjectList);
	void Render(CCameraComponent* aCamera, CEnvironmentLight* anEnvironmentLight);
	void Render(CCameraComponent* aCamera, std::vector<CPointLight*>& aPointLightList);

	bool ToggleRenderPass();

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

	bool CreateVertexShader(std::string aFilepath, CDirectXFramework* aFramework, ID3D11VertexShader* outVertexShader);
	bool CreatePixelShader(std::string aFilepath, CDirectXFramework* aFramework, ID3D11PixelShader* outPixelShader);

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
		unsigned int myPaddington[2];
	} myObjectBufferData;

	struct SLightBufferData 
	{
		DirectX::SimpleMath::Vector4 myDirectionalLightDirection;
		DirectX::SimpleMath::Vector4 myDirectionalLightColor;
	} myLightBufferData;

	struct SPointLightBufferData 
	{
		DirectX::SimpleMath::Vector4 myColorAndIntensity;
		DirectX::SimpleMath::Vector4 myPositionAndRange;
	} myPointLightBufferData;

private:
	bool LoadRenderPassPixelShaders(ID3D11Device* aDevice);

	ID3D11DeviceContext* myContext;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;
	ID3D11Buffer* myLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	ID3D11VertexShader* myFullscreenShader;
	ID3D11VertexShader* myModelVertexShader;
	ID3D11VertexShader* myInstancedModelVertexShader;
	ID3D11PixelShader* myGBufferPixelShader;
	ID3D11PixelShader* myEnvironmentLightShader;
	ID3D11PixelShader* myPointLightShader;
	ID3D11SamplerState* mySamplerState;

	std::vector<ID3D11PixelShader*> myRenderPassPixelShaders;
	ID3D11PixelShader* myCurrentRenderPassPixelShader;
	unsigned short myRenderPassIndex;
};

