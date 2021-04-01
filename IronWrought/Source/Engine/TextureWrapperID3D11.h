#pragma once
class CTextureWrapperID3D11
{
public:
	CTextureWrapperID3D11();
	CTextureWrapperID3D11(const std::string& aTexturePath);
	CTextureWrapperID3D11(const char* aTexturePath);

	CTextureWrapperID3D11(const CTextureWrapperID3D11& other);
	CTextureWrapperID3D11 operator=(const CTextureWrapperID3D11& other);

	~CTextureWrapperID3D11();

	void Init(const std::string& aTexturePath);
	void Init(const char* aTexturePath);

	ID3D11ShaderResourceView* ShaderResource();
	void ShaderResource(ID3D11ShaderResourceView*);

private:
	bool VerifyIsDDS(const std::string& aPathToVerify);
	bool VerifyIsDDS(const char* aPathToVerify);

private:
	ID3D11ShaderResourceView* myShaderResource;
	std::string myName;
};

