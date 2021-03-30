#pragma once

class CDirectXFramework;
class CEnvironmentLight;
class CBoxLight;
class CCameraComponent;
class CGameObject;

class CShadowRenderer
{
public:
	CShadowRenderer();
	~CShadowRenderer();

	bool Init(CDirectXFramework* aFramework);
	void Render(CEnvironmentLight* anEnvironmentLight, std::vector<CGameObject*>& aGameObjectList, std::vector<CGameObject*>& aInstancedGameObjectList);
	void Render(CBoxLight* aBoxLightList, std::vector<CGameObject*>& aGameObjectList, std::vector<CGameObject*>& aInstancedGameObjectList);

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
	} myObjectBufferData;

	struct SBoneBufferData {
		Matrix myBones[64];
	} myBoneBufferData;

private:
	ID3D11DeviceContext* myContext;
	ID3D11SamplerState* myShadowSampler;
	ID3D11Buffer* myFrameBuffer;
	ID3D11Buffer* myObjectBuffer;
	ID3D11Buffer* myBoneBuffer;
	ID3D11VertexShader* myModelVertexShader;
	ID3D11VertexShader* myAnimationVertexShader;
	ID3D11VertexShader* myInstancedModelVertexShader;
};

