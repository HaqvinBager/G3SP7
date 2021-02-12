#pragma once
#include "Behaviour.h"

class CGameObject;
class CDecal;

class CDecalComponent : public CBehaviour
{
public:
	CDecalComponent(CGameObject& aParent, const std::string& aFBXPath);
	~CDecalComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	CDecal* GetMyDecal() const;

	void OnEnable() override;
	void OnDisable() override;

private:
	CDecal* myDecal;
	std::string myDecalPath;
};

