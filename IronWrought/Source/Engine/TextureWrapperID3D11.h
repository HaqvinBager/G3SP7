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
	ID3D11ShaderResourceView* const* ConstShaderResource();
	void ShaderResource(ID3D11ShaderResourceView* aTexture, const std::string& aTexturePath = "");

	inline const bool HasTexture() const { return myShaderResource != nullptr; };

	const std::string& TextureName() const { return myName; }

private:
	bool VerifyIsDDS(const std::string& aPathToVerify);
	bool VerifyIsDDS(const char* aPathToVerify);

private:
	ID3D11ShaderResourceView* myShaderResource;
	std::string myName;

public:
#ifndef NDEBUG
public:
	// Only usable in _DEBUG! Might be necessary to change later.
	const std::string& TexturePath() { return myTexturePath; }
private:
	std::string myTexturePath;
#else
public:
	std::string TexturePath() { std::string error = (__FUNCTION__); error.append(" ERROR: Function used outside of DEBUG");  return std::move(error); }
#endif
};

