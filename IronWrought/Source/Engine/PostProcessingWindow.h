#pragma once
#include "Window.h"

namespace IronWroughtImGui {

	enum class EToneMappingAlgorithm
	{
		Reinhard,
		Uncharted,
		ACES
	};

	class CPostProcessingWindow : public CWindow
	{
	public:
		CPostProcessingWindow(const char* aName);
		~CPostProcessingWindow() override;
		void OnEnable() override;
		void OnInspectorGUI() override;
		void OnDisable() override;

	private:
		Vector4 myWhitePointColor;
		float myWhitePointIntensity;
		float myExposure;
		bool myIsReinhard;
		bool myIsUncharted;
		bool myIsACES;

		float mySSAORadius;
		float mySSAOSampleBias;
		float mySSAOMagnitude;
		float mySSAOContrast;
		float mySSAOConstantBias;
	};

}