#pragma once
#include "Behaviour.h"

namespace physx
{
	class PxShape;
	class PxMaterial;
}

class CScene;

class CLineInstance;

#ifdef _DEBUG
#define DEBUG_COLLIDER_BOX // DOES NOT WORK PROPERLY, UPDATE TRANSFORM 
#endif

class CBoxColliderComponent : public CBehaviour
{
public:
	enum class EEventFilter
	{
		PlayerOnly,
		EnemyOnly,
		ObjectOnly,
		Any,
		None
	};

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
	void RegisterEventTriggerFilter(const int& anEventFilter);
	//const SStringMessage& EventTriggerMessage() { return myTriggerMessage; }

	void OnEnable() override;
	void OnDisable() override;

private:
	void CreateColliderDraw(const float& aHalfSizeX, const float& aHalfSizeY, const float& aHalfSizeZ, const Vector3& aPosOffset);

private:
	physx::PxShape* myShape;
	physx::PxMaterial* myMaterial;
	Vector3 myPositionOffset;
	Vector3 myBoxSize;
	bool myIsTrigger;

	std::string myEventMessage;
	EEventFilter myEventFilter;

#ifdef DEBUG_COLLIDER_BOX
	CLineInstance* myColliderDraw;
#endif
};
