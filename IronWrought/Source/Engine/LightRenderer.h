#pragma once

struct ID3D11DeviceContext;
struct ID3D11Buffer;
class CDirectXFramework;
class CEnvironmentLight;
class CPointLight;
class CSpotLight;
class CBoxLight;
class CCameraComponent;

class CLightRenderer
{
public:
	CLightRenderer();
	~CLightRenderer();

	bool Init(CDirectXFramework* aFramework);

	void Render(CCameraComponent* aCamera, CEnvironmentLight* anEnvironmentLight);
	void Render(CCameraComponent* aCamera, std::vector<CPointLight*>& aPointLightList);
	void Render(CCameraComponent* aCamera, std::vector<CSpotLight*>& aSpotLightList);
	void Render(CCameraComponent* aCamera, std::vector<CBoxLight*>& aBoxLightList);

	void RenderVolumetric(CCameraComponent* aCamera, CEnvironmentLight* anEnvironmentLight);
	void RenderVolumetric(CCameraComponent* aCamera, std::vector<CPointLight*>& aPointLightList);
	void RenderVolumetric(CCameraComponent* aCamera, std::vector<CSpotLight*>& aSpotLightList);
	void RenderVolumetric(CCameraComponent* aCamera, std::vector<CBoxLight*>& aBoxLightList);

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
		Matrix myToCameraSpace;
		Matrix myToWorldFromCamera;
		Matrix myToProjectionSpace;
		Matrix myToCameraFromProjection;
		Vector4 myCameraPosition;
	} myFrameBufferData;

	struct SDirectionalLightBufferData
	{
		Matrix myToDirectionalLightView;
		Matrix myToDirectionalLightProjection;
		Vector4 myDirectionalLightPosition;
		Vector4 myDirectionalLightDirection;
		Vector4 myDirectionalLightColor;
	} myDirectionalLightBufferData;

	struct SPointLightBufferData
	{
		Matrix myToWorldSpace;
		Vector4 myColorAndIntensity;
		Vector4 myPositionAndRange;
	} myPointLightBufferData;

	struct SSpotLightBufferData
	{
		Matrix myToWorldSpace;
		Matrix myToViewSpace;
		Matrix myToProjectionSpace;
		Vector4 myColorAndIntensity;
		Vector4 myPositionAndRange;
		Vector4 myDirectionAndAngleExponent;
		Vector4 myDirectionNormal1;
		Vector4 myDirectionNormal2;
		Vector4 myUpLeftCorner;
		Vector4 myUpRightCorner;
		Vector4 myDownLeftCorner;
		Vector4 myDownRightCorner;
	} mySpotLightBufferData;

	struct SBoxLightBufferData
	{
		Matrix myToWorldSpace;
		Matrix myToViewSpace;
		Matrix myToProjectionSpace;
		Vector4 myColorAndIntensity;
		Vector4 myPositionAndRange;
		Vector4 myDirection;
		//Vector2 myWidthAndHeight;
		//Vector2 myPadding;
	} myBoxLightBufferData;

private:
	ID3D11DeviceContext* myContext;

	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myLightBuffer;
	ID3D11Buffer* myPointLightBuffer;
	ID3D11Buffer* mySpotLightBuffer;
	ID3D11Buffer* myBoxLightBuffer;

	ID3D11Buffer* myPointLightVertexBuffer;
	ID3D11Buffer* myPointLightIndexBuffer;
	ID3D11Buffer* mySpotLightVertexBuffer;
	ID3D11Buffer* myBoxLightVertexBuffer;
	ID3D11Buffer* myBoxLightIndexBuffer;

	ID3D11InputLayout* myInputLayout;

	ID3D11VertexShader* myFullscreenShader;
	ID3D11VertexShader* myPointLightVertexShader;
	ID3D11VertexShader* mySpotLightVertexShader;
	ID3D11VertexShader* myBoxLightVertexShader;

	ID3D11GeometryShader* mySpotLightGeometryShader;

	ID3D11PixelShader* myEnvironmentLightShader;
	ID3D11PixelShader* myPointLightShader;
	ID3D11PixelShader* mySpotLightShader;
	ID3D11PixelShader* myBoxLightShader;
	ID3D11PixelShader* myDirectionalVolumetricLightShader;
	ID3D11PixelShader* myPointVolumetricLightShader;
	ID3D11PixelShader* mySpotVolumetricLightShader;
	ID3D11PixelShader* myBoxLightVolumetricLightShader;

	ID3D11SamplerState* mySamplerState;
	ID3D11SamplerState* myShadowSampler;

	UINT myPointLightNumberOfVertices;
	UINT myPointLightNumberOfIndices;
	UINT myPointLightStride;
	UINT myPointLightOffset;
};

