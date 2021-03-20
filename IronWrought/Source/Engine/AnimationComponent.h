#pragma once
#include "Behaviour.h"
#include "StringID.hpp"
#include "ModelMath.h"
#include "assimp/matrix4x4.h"

class CAnimationController;
class CGameObject;
class CAnimationComponent : public CBehaviour
{
public:
	CAnimationComponent(CGameObject& aParent, const std::string& aModelFilePath, std::vector<std::string>& someAnimationPaths);
	~CAnimationComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnEnable() override;
	void OnDisable() override;

public:
	void SetBones(std::array<aiMatrix4x4, 64> someBones) { myBones = someBones; };
	void SetBonesToIdentity();
	std::array<aiMatrix4x4, 64> GetBones() { return myBones; }
	CAnimationController* GetController() { return myController; }

private:
	CAnimationController* myController;
	std::array<aiMatrix4x4, 64> myBones{ };
};