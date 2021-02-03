#pragma once
#include <vector>
#include <string>

struct ImFontAtlas;
class CImguiManager
{
public:
	CImguiManager();
	~CImguiManager();
	void DebugWindow();
	void LevelSelect();
	void LevelsToSelectFrom(std::vector<std::string> someLevelsToSelectFrom);
private:
	const std::string GetSystemMemory();
	const std::string GetDrawCalls();
	std::vector<std::string> myLevelsToSelectFrom;
};
