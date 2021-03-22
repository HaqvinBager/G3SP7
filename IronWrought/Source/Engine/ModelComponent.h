#pragma once
#include "Behaviour.h"

class CGameObject;
class CModel;

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
	bool SetTints(std::vector<Vector3>& aVectorWithTints);
	const std::vector<Vector3>& GetTints();

	// Primary tint.
	void Tint1(const Vector3& aTint);
	// Secondary tint.
	void Tint2(const Vector3& aTint);
	// Tertiary tint.
	void Tint3(const Vector3& aTint);
	// Accents tint.
	void Tint4(const Vector3& aTint);

	// Primary tint.
	Vector4 Tint1() const;
	// Secondary tint.
	Vector4 Tint2() const;
	// Tertiary tint.
	Vector4 Tint3() const;
	// Accents tint.
	Vector4 Tint4() const;

	void HasTintMap(const bool aHasTintMap);

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

	std::vector<Vector3> myTints;
};