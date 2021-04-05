#pragma once
#include "Behaviour.h"

#include "TextureWrapperID3D11.h"

class CGameObject;
class CModel;

#define NUMBER_OF_TINT_SLOTS 4

class CModelComponent : public CBehaviour
{
public:
	CModelComponent(CGameObject& aParent, const std::string& aFBXPath);
	~CModelComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void SetModel(const std::string& aFBXPath);

	CModel *GetMyModel() const;
	const std::string& GetModelPath() const { return myModelPath; }

	void OnEnable() override;
	void OnDisable() override;

public:
	bool SetTints(std::vector<Vector4>& aVectorWithTints);
	std::vector<Vector4> GetTints();

	// Primary tint.
	void Tint1(const Vector4& aTint);
	// Secondary tint.
	void Tint2(const Vector4& aTint);
	// Tertiary tint.
	void Tint3(const Vector4& aTint);
	// Accents tint.
	void Tint4(const Vector4& aTint);
	void Emissive(const Vector4& aTint);

	// Primary tint.
	const Vector4& Tint1() const;
	// Secondary tint.
	const Vector4& Tint2() const;
	// Tertiary tint.
	const Vector4& Tint3() const;
	// Accents tint.
	const Vector4& Tint4() const;
	const Vector4& Emissive() const;

	ID3D11ShaderResourceView* Texture1();
	ID3D11ShaderResourceView* Texture2();
	ID3D11ShaderResourceView* Texture3();
	ID3D11ShaderResourceView* Texture4();
	ID3D11ShaderResourceView* TextureOnIndex(const int& anIndex);
	ID3D11ShaderResourceView* const* ConstTintTexture1();
	ID3D11ShaderResourceView* const* ConstTintTexture2();
	ID3D11ShaderResourceView* const* ConstTintTexture3();
	ID3D11ShaderResourceView* const* ConstTintTexture4();
	ID3D11ShaderResourceView* const* ConstTintTextureOnIndex(const int& anIndex);

	void Texture1(const std::string& aTexturePath);
	void Texture2(const std::string& aTexturePath);
	void Texture3(const std::string& aTexturePath);
	void Texture4(const std::string& aTexturePath);
	void TextureOnIndex(const std::string& aTexturePath, const int anIndex);
	void Texture1(ID3D11ShaderResourceView* aTexture);
	void Texture2(ID3D11ShaderResourceView* aTexture);
	void Texture3(ID3D11ShaderResourceView* aTexture);
	void Texture4(ID3D11ShaderResourceView* aTexture);
	void TextureOnIndex(ID3D11ShaderResourceView* aTexture, const int anIndex);

	struct STintData
	{
		CTextureWrapperID3D11 myTexture;
		Vector4 myColor;
	};
	bool SetTintTextures(std::vector<STintData>& aVectorWithTintTextures);
	const std::vector<STintData>& GetTintData();
	std::array<ID3D11ShaderResourceView*, 4> GetTintTextures();
	std::array<ID3D11ShaderResourceView* const*, 4> GetConstTintTextures();

	// This is confusingly named.
	void HasTintMap(const bool aHasTintMap);

	std::array<bool, 4> HasTintTexturesOnSlots() const;

public:
	const unsigned int VertexPaintColorID() const;
	const std::vector<std::string>& VertexPaintMaterialNames() const;
	const bool RenderWithAlpha() const { return myRenderWithAlpha; }

private:
	CModel* myModel;
	std::string myModelPath;
	std::vector<std::string> myVertexPaintMaterialNames;
	unsigned int myVertexPaintColorID;
	bool myRenderWithAlpha;

	std::vector<STintData> myTints;
	Vector4 myEmissive;
};