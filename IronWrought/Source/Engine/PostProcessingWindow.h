#pragma once
#include "Window.h"

namespace IronWroughtImGui {

	class CPostProcessingWindow : public CWindow
	{
	public:
		CPostProcessingWindow(const char* aName);
		~CPostProcessingWindow() override;
		void OnEnable() override;
		void OnInspectorGUI() override;
		void OnDisable() override;

	private:
		float mySSAORadius;
		float mySSAOSampleBias;
		float mySSAOMagnitude;
		float mySSAOContrast;
		float mySSAOConstantBias;
	};

}