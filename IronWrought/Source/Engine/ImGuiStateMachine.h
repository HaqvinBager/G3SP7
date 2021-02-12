#pragma once
#include "ImGuiWindow.h"
class CStateMachineComponent;

class ImGuiStateMachine : public CImGuiWindow
{
public:
	ImGuiStateMachine(const char* aWindowName);
	~ImGuiStateMachine();

	void OnInspectorGUI() override;

private:
	void ShowAddStateButton(CStateMachineComponent* aStateMachine);

private:
	struct SAddStateData {
		bool myAddStateFlag;
		bool myAddStateNameFlag;
		char bufName[64];
	} myAddStateData;
};

