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
		OrbitCamera,
		UnlockCursor
	};

public:
	CCameraControllerComponent(CGameObject& gameObject,  float aCameraMoveSpeed = 2.0f, ECameraMode aCameraMode = ECameraMode::OrbitCamera, char aToggleFreeCam = VK_F4, DirectX::SimpleMath::Vector3 aOffset = {0.f, 0.f, 0.f});
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
	void UpdateOrbitCam();

	const char myToggleFreeCam;
	ECameraMode myCameraMode;
	float myCameraMoveSpeed;
	CCameraComponent* myCamera;
	DirectX::SimpleMath::Vector3 myOffset;

	float myMouseRotationSpeed;
	float myPitch;
	float myYaw;
	
	//Controls the cameras position in space
		//Distance from origin
		float r = 10.0f;
		//Rotates the camera around the origin, rotation around the equator
		float theta = 0.0f;
		//Rotates the camera around the origin, rotation towards north and southpole
		float phi = 0.0f;
	//Controls camera orientation
		float pitch = 0.0f;
		float yaw = 0.0f;
		float roll = 0.0f;

};
