#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
	class PxMaterial;
}

class CScene;

class CBoxColliderComponent : public CBehaviour
{
public:
	CBoxColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const Vector3& aBoxSize, bool aIsStatic = false, physx::PxMaterial* aMaterial = nullptr);
	~CBoxColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void CreateBoxCollider();

	void RegisterEventTriggerMessage(const SMessage& aMessage) { myTriggerMessages.push_back(aMessage); }
	const std::vector<SMessage>& EventTriggerMessage() { return myTriggerMessages; }

	void OnEnable() override;
	void OnDisable() override;
private:
	physx::PxShape* myShape;
	physx::PxMaterial* myMaterial;
	Vector3 myPositionOffset;
	Vector3 myBoxSize;

	std::vector<SMessage> myTriggerMessages;
};

