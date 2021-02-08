#include "Component.h"

class CCameraComponent;
class CGameObject;

class CCameraControllerComponent : public CComponent
{
public:
	enum class ECameraMode
	{
		Player,
		FreeCam,
		MenuCam
	};

public:
	CCameraControllerComponent(CGameObject& gameObject,  float aCameraMoveSpeed = 5.0f, ECameraMode aCameraMode = ECameraMode::FreeCam, char aToggleFreeCam = VK_F1, DirectX::SimpleMath::Vector3 aOffset = {0.f, 0.f, 0.f});
	~CCameraControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;


private:
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
