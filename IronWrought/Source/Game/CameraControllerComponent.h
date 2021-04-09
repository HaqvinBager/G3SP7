#include "Component.h"

class CCameraComponent;
class CGameObject;
#define PI 3.14159265f
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
	CCameraControllerComponent(CGameObject& gameObject,  float aCameraMoveSpeed = 2.0f, ECameraMode aCameraMode = ECameraMode::FreeCam, char aToggleFreeCam = VK_F4, DirectX::SimpleMath::Vector3 aOffset = {0.f, 0.f, 0.f});
	~CCameraControllerComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	float GetCameraMoveSpeed();
	void SetCameraMoveSpeed(float aCameraMoveSpeed);
	void SetCameraMode(const ECameraMode& aCameraMode) { myCameraMode = aCameraMode; }
	const bool IsFreeCamMode() const { return (myCameraMode == ECameraMode::FreeCam); }
	const bool IsCursorUnlocked() const { return (myCameraMode == ECameraMode::UnlockCursor); }
	void RotateTransformWithYawAndPitch(const Vector2& aInput);


public:
	float WrapAngle(float anAngle)
	{
		return fmodf(anAngle, 360.0f);
	}

	float ToDegrees(float anAngleInRadians)
	{
		return anAngleInRadians * (180.0f / PI);
	}

	float ToRadians(float anAngleInDegrees)
	{
		return anAngleInDegrees * (PI / 180.0f);
	}

	float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	float LerpDegrees(float a, float b, float lerpFactor) // Lerps from angle a to b (both between 0.f and 360.f), taking the shortest path
	{
		float result;
		float diff = b - a;
		if (diff < -180.f)
		{
			// lerp upwards past 360
			b += 360.f;
			result = Lerp(a, b, lerpFactor);
			if (result >= 360.f)
			{
				result -= 360.f;
			}
		}
		else if (diff > 180.f)
		{
			// lerp downwards past 0
			b -= 360.f;
			result = Lerp(a, b, lerpFactor);
			if (result < 0.f)
			{
				result += 360.f;
			}
		}
		else
		{
			// straight lerp
			result = Lerp(a, b, lerpFactor);
		}

		return result;
	}

	// Creates a camera and sets parent-child relationship with proper offsets.
	static CGameObject* CreatePlayerFirstPersonCamera(CGameObject* aParentObject);


private:
	void UpdatePlayerFirstPerson();
	void UpdateFreeCam();

	const char myToggleFreeCam;
	ECameraMode myCameraMode;
	ECameraMode myPrevCameraMode;
	float myCameraMoveSpeed;
	CCameraComponent* myCamera;
	DirectX::SimpleMath::Vector3 myOffset;
#ifdef _DEBUG
	Vector3 myPositionBeforeFreeCam;
#endif
	float myMouseRotationSpeed;
	float myPitch;
	float myYaw;
};
