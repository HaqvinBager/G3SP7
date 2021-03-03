#include "Component.h"

class CCameraComponent;
class CGameObject;

class CCameraControllerComponent : public CComponent
{
public:
	enum class ECameraMode
	{
		PlayerFirstPerson,
		FreeCam,
		MenuCam,
		UnlockCursor
	};

public:
	CCameraControllerComponent(CGameObject& gameObject,  float aCameraMoveSpeed = 5.0f, ECameraMode aCameraMode = ECameraMode::FreeCam, char aToggleFreeCam = VK_F4, DirectX::SimpleMath::Vector3 aOffset = {0.f, 0.f, 0.f});
	~CCameraControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	float GetCameraMoveSpeed();
	void SetCameraMoveSpeed(float aCameraMoveSpeed);
public:
	// Creates a camera and sets parent-child relationship with proper offsets.
	static CGameObject* CreatePlayerFirstPersonCamera(CGameObject* aParentObject);


private:
	void UpdatePlayerFirstPerson();
	void UpdateFreeCam();

	const char myToggleFreeCam;
	ECameraMode myCameraMode;
	float myCameraMoveSpeed;
	CCameraComponent* myCamera;
	DirectX::SimpleMath::Vector3 myOffset;

	float myMouseRotationSpeed;
	float myPitch;
	float myYaw;
};
