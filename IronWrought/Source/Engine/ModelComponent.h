#pragma once
#include "Behaviour.h"

class CModelInstance;
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

	void OnEnable() override;
	void OnDisable() override;
private:
	CModel* myModel;
};