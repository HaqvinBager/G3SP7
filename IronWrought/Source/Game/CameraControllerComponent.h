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
	union
	{
		float myPitch;
		float myTheta;//Rotates the camera around the origin, rotation around the equator. Used for Orbit Camera.
	};
	union
	{
		float myYaw;
		float myPhi;//Rotates the camera around the origin, rotation towards north and southpole. Used for Orbit Camera.
	};
	float myOrbitRadius;// Distance from origin 
	Vector3 myOrbitCenter;
	// Not used.
	//float myOrbitPitch;
	//float myOrbitYaw;
	//float myOrbitRoll;
};
