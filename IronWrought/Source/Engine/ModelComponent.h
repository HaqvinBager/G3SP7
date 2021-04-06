#pragma once
#include "Behaviour.h"

#include "TextureWrapperID3D11.h"

#include <array>

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
	const unsigned int VertexPaintColorID() const;
	const std::vector<std::string>& VertexPaintMaterialNames() const;
	const bool RenderWithAlpha() const { return myRenderWithAlpha; }

#pragma region TINT_FUNCTIONS
public:
	bool SetTints(std::vector<Vector4>& aVectorWithTints);
	std::vector<Vector4> GetTints();

	void TintOnIndex(const Vector4& aTint, const int& anIndex);
	// Primary tint.
	void Tint1(const Vector4& aTint);
	// Secondary tint.
	void Tint2(const Vector4& aTint);
	// Tertiary tint.
	void Tint3(const Vector4& aTint);
	// Accents tint.
	void Tint4(const Vector4& aTint);
	void Emissive(const Vector4& aTint);

	const Vector4& TintOnIndex(const int& anIndex);
	// Primary tint.
	const Vector4& Tint1() const;
	// Secondary tint.
	const Vector4& Tint2() const;
	// Tertiary tint.
	const Vector4& Tint3() const;
	// Accents tint.
	const Vector4& Tint4() const;
	const Vector4& Emissive() const;

	ID3D11ShaderResourceView* TintTextureOnIndex(const int& anIndex);
	ID3D11ShaderResourceView* const* ConstTintTextureOnIndex(const int& anIndex);
	std::array<ID3D11ShaderResourceView*, 4> GetTintTextures();
	std::array<ID3D11ShaderResourceView* const*, 4> GetConstTintTextures();

	void TintTextureOnIndex(const std::string& aTexturePath, const int& anIndex);
	void TintTextureOnIndex(ID3D11ShaderResourceView* aTexture, const int& anIndex);

	const std::string& TextureNameOnIndex(const int& anIndex);
	const std::string& TexturePathOnIndex(const int& anIndex);

	struct STintData
	{
		CTextureWrapperID3D11 myTexture;
		Vector4 myColor;
	};
	bool SetTintTextures(std::vector<STintData>& aVectorWithTintTextures);
	const std::vector<STintData>& GetTintData();

	// This is confusingly named.
	void HasTintMap(const bool aHasTintMap);

	bool SerializeTintData(const std::string& aTintDataPath = "");
	bool DeserializeTintData(const std::string& aTintDataPath);

private:
	inline int TintsBoundsCheck(const int& anIndex);
#pragma endregion TINT_FUNCTIONS

private:
	CModel* myModel;
	std::string myModelPath;
	std::vector<std::string> myVertexPaintMaterialNames;
	unsigned int myVertexPaintColorID;
	bool myRenderWithAlpha;

	std::vector<STintData> myTints;
	Vector4 myEmissive;
};