#pragma once
#include <vector>
#include <string>

class CGraphManager;
class ImGuiStateMachine;
struct ImFontAtlas;

class CImguiManager
{
public:
	CImguiManager();
	~CImguiManager();
	void Update();
	void PostRender();
	void DebugWindow();
	void LevelSelect();
	void LevelsToSelectFrom(std::vector<std::string> someLevelsToSelectFrom);
private:
	CGraphManager* myGraphManager;
	const std::string GetSystemMemory();
	const std::string GetDrawCalls();
	std::vector<std::string> myLevelsToSelectFrom;
	bool myGraphManagerIsFullscreen;
	bool myIsEnabled;

	ImGuiStateMachine* myStateMachine;
};
