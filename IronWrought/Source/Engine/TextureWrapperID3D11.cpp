#include "stdafx.h"
#include "TextureWrapperID3D11.h"

#include "MainSingleton.h"
#include "MaterialHandler.h"

CTextureWrapperID3D11::CTextureWrapperID3D11()
	: myShaderResource(nullptr)
	, myTexturePath("")
{}

CTextureWrapperID3D11::CTextureWrapperID3D11(const std::string & aTexturePath)
{
	Init(aTexturePath);
}

CTextureWrapperID3D11::CTextureWrapperID3D11(const char* aTexturePath)
{
	Init(std::string(aTexturePath));
}

CTextureWrapperID3D11::CTextureWrapperID3D11(const CTextureWrapperID3D11 & other)
{
	myName = other.myName;
	myShaderResource = other.myShaderResource;
}

CTextureWrapperID3D11 CTextureWrapperID3D11::operator=(const CTextureWrapperID3D11 & other)
{
	myName = other.myName;
	myShaderResource = other.myShaderResource;
	return *this;
}

CTextureWrapperID3D11::~CTextureWrapperID3D11()
{
	myShaderResource = nullptr;
	CMainSingleton::MaterialHandler().ReleaseSingleTexture(myName);
	myName = "";
	myTexturePath = "";
}

void CTextureWrapperID3D11::Init(const std::string & aTexturePath)
{
	assert(VerifyIsDDS(aTexturePath) && std::string(aTexturePath + " is not .dds format.").c_str());
	if (VerifyIsDDS(aTexturePath))
	{
#ifndef NDEBUG
		myTexturePath = aTexturePath;
#endif // !NDEBUG

		size_t lastSlash = aTexturePath.find_last_of("\\/");
		size_t lastDot = aTexturePath.find_last_of(".");
		size_t length = aTexturePath.length() - lastDot;
		myName = aTexturePath.substr(lastSlash + 1, length - lastSlash + 1);
		lastDot = myName.find_last_of(".");
		myName = myName.substr(0, lastDot);
		myShaderResource = CMainSingleton::MaterialHandler().RequestSingleTexture(myName);
	}
}

void CTextureWrapperID3D11::Init(const char* aTexturePath)
{
	Init(std::string(aTexturePath));
}

void CTextureWrapperID3D11::Clear()
{
	myShaderResource = nullptr;
	CMainSingleton::MaterialHandler().ReleaseSingleTexture(myName);
	myName = "";
	myTexturePath = "";
}

ID3D11ShaderResourceView* CTextureWrapperID3D11::ShaderResource()
{
	return myShaderResource;
}

ID3D11ShaderResourceView* const* CTextureWrapperID3D11::ConstShaderResource()
{
	return &myShaderResource;
}

void CTextureWrapperID3D11::ShaderResource(ID3D11ShaderResourceView* aTexture, const std::string& aTexturePath)
{
#ifndef NDEBUG
	myTexturePath = aTexturePath;
#else
	aTexturePath;
#endif // !NDEBUG

	myShaderResource = aTexture;
}

bool CTextureWrapperID3D11::VerifyIsDDS(const std::string & aPathToVerify)
{
	size_t lastDot = aPathToVerify.find_last_of(".");
	return aPathToVerify.substr(lastDot, aPathToVerify.length() - lastDot) == ".dds";
}

bool CTextureWrapperID3D11::VerifyIsDDS(const char* aPathToVerify)
{
	return VerifyIsDDS(std::string(aPathToVerify));
}
