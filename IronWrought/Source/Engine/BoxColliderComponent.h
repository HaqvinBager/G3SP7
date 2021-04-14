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
	CBoxColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const Vector3& aBoxSize, const bool aIsTrigger, physx::PxMaterial* aMaterial = nullptr);
	~CBoxColliderComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void CreateBoxCollider();

	void OnTriggerEnter();
	void OnTriggerExit();
	void RegisterEventTriggerMessage(const std::string& aMessage) { myEventMessage = aMessage; }
	//const SStringMessage& EventTriggerMessage() { return myTriggerMessage; }

	void OnEnable() override;
	void OnDisable() override;
private:
	physx::PxShape* myShape;
	physx::PxMaterial* myMaterial;
	Vector3 myPositionOffset;
	Vector3 myBoxSize;
	bool myIsTrigger;

	std::string myEventMessage;
};
