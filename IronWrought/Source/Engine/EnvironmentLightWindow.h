#pragma once
#include "Window.h"

class CEnvironmentLightComponent;
class CGameObject;

namespace IronWroughtImGui {
	class CEnvironmentLightWindow : public CWindow
	{
	public:
		CEnvironmentLightWindow(const char* aName);
		~CEnvironmentLightWindow() override;
		void OnEnable() override;
		void OnInspectorGUI() override;
		void OnDisable() override;

	private:
		Vector3 myColor;
		float myIntensity;
		float myNumberOfSamples;
		float myLightPower;
		float myScatteringProbability;
		float myHenyeyGreensteinGValue;
		bool myIsVolumetric;
		bool myIsFog;
	};
}