#pragma once
#include <imgui.h>
#include <ImGui\Core\imgui_internal.h>

class CImGuiWindow {
public:
	CImGuiWindow(const char* aWindowName) : myWindowName(aWindowName), myIsEnabled(false) { }
	
	virtual ~CImGuiWindow() { }

	virtual void OnInspectorGUI() = 0;

	//void ToggleActive() { myIsEnabled = !myIsEnabled; }
	const bool IsOpen() const { return myIsEnabled; }

	bool* IsOpen() { return &myIsEnabled; }

	const char* Name() const { return myWindowName; }

private:
	const char* myWindowName;
	bool myIsEnabled;
};