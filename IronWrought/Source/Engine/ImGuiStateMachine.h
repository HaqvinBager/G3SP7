#pragma once

class CStateMachineComponent;

class ImGuiStateMachine
{
public:
	ImGuiStateMachine();
	~ImGuiStateMachine();

	void OnInspectorGUI();
	void ToggleStateMachine() { myIsEnabled = !myIsEnabled; }
	const bool ShouldRender() const { return myIsEnabled; }

private:
	void ShowAddStateButton(CStateMachineComponent* aStateMachine);

private:
	bool myIsEnabled;

	struct SAddStateData {
		bool myAddStateFlag;
		bool myAddStateNameFlag;
		char bufName[64];
	} myAddStateData;
};

