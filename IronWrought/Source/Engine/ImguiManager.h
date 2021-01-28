#pragma once
struct ImFontAtlas;
class CImguiManager
{
public:
	CImguiManager();
	~CImguiManager();
	void DebugWindow();
private:
	const std::string GetSystemMemory();
	const std::string GetDrawCalls();
	CImGuiLevelSelect* mySelectLevelWindow;
};
