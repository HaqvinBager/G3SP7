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

private:
	struct SFrameBufferData
	{
		DirectX::SimpleMath::Matrix myToCameraSpace;
		DirectX::SimpleMath::Matrix myToWorldFromCamera;
		DirectX::SimpleMath::Matrix myToProjectionSpace;
		DirectX::SimpleMath::Matrix myToCameraFromProjection;
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
		DirectX::SimpleMath::Matrix myDirectionalLightTransform;
		DirectX::SimpleMath::Matrix myDirectionalLightView;
		DirectX::SimpleMath::Vector4 myDirectionalLightPosition;
		DirectX::SimpleMath::Vector4 myDirectionalLightDirection;
		DirectX::SimpleMath::Vector4 myDirectionalLightColor;
	} myLightBufferData;

	struct SPointLightBufferData 
	{
		DirectX::SimpleMath::Vector4 myColorAndIntensity;
		DirectX::SimpleMath::Vector4 myPositionAndRange;
	} myPointLightBufferData;

	struct SBoneBufferData {
		/*SlimMatrix44*/Matrix myBones[64];
	} myBoneBufferData;

	static_assert((sizeof(SBoneBufferData) % 16) == 0, "SBoneBufferData size not padded correctly");

private:
	bool LoadRenderPassPixelShaders(ID3D11Device* aDevice);

	ID3D11DeviceContext* myContext;
// Buffers;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;
	ID3D11Buffer* myLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	ID3D11Buffer* myBoneBuffer;

	ID3D11InputLayout* myVertexPaintInputLayout;
// Vertex shaders.
	ID3D11VertexShader* myFullscreenShader;
	ID3D11VertexShader* myModelVertexShader;
	ID3D11VertexShader* myAnimationVertexShader;
	ID3D11VertexShader* myVertexPaintModelVertexShader;
	ID3D11VertexShader* myInstancedModelVertexShader;
// Pixel shaders.
	ID3D11PixelShader* myGBufferPixelShader;
	ID3D11PixelShader* myVertexPaintPixelShader;
	ID3D11PixelShader* myEnvironmentLightShader;
	ID3D11PixelShader* myPointLightShader;
// Samplers.
	ID3D11SamplerState* mySamplerState;
	ID3D11SamplerState* myShadowSampler;

	std::vector<ID3D11PixelShader*> myRenderPassShaders;
	ID3D11PixelShader* myCurrentRenderPassShader;
	// Switches between myRenderPassGBuffer and myGBufferPixelShader
	ID3D11PixelShader* myCurrentGBufferPixelShader;
	ID3D11PixelShader* myRenderPassGBuffer;
	unsigned short myRenderPassIndex;
};

