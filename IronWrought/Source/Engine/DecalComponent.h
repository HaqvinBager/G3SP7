#pragma once
#include "Behaviour.h"

class CGameObject;
class CModel;

class CDecalComponent : public CBehaviour
{
public:
	CDecalComponent(CGameObject& aParent, const std::string& aFBXPath);
	~CDecalComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void SetModel(const std::string& aFBXPath);

	CModel* GetMyModel() const;

	void OnEnable() override;
	void OnDisable() override;

private:
	CModel* myModel;
	std::string myModelPath;
};

