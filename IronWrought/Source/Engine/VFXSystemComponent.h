#pragma once
#include "Behaviour.h"

class CGameObject;
class CVFXBase;
class CVFXSystemComponent : public CBehaviour
{
public:
	CVFXSystemComponent(CGameObject& aParent, const std::vector<std::string>& someVFXPaths, const std::vector<Matrix>& someTransforms);
	~CVFXSystemComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	void OnEnable() override;
	void OnDisable() override;

public:
	std::vector<CVFXBase*>& GetVFXBases() { return myVFXBases; }
	std::vector<Matrix>& GetVFXTransforms() { return myVFXTransforms; }

private:
	std::vector<float> myVFXDelays;
	std::vector<float> myVFXDurations;
	std::vector<CVFXBase*> myVFXBases;
	std::vector<Matrix> myVFXTransforms;
	std::vector<Matrix> myVFXTransformsOriginal;

};

