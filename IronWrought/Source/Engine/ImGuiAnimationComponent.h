#pragma once
#include "ImGuiWindow.h"

class ImGuiAnimationComponent : public CImGuiWindow
{
public:
	ImGuiAnimationComponent(const char* aWindowName);
	~ImGuiAnimationComponent() override;
	void OnInspectorGUI() override;
};

