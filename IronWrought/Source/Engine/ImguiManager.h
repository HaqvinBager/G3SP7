#pragma once

class CImGuiLevelSelect;

class CImguiManager
{
public:
	CImguiManager();
	~CImguiManager();
	void DebugWindow();

private:
	CImGuiLevelSelect* mySelectLevelWindow;
};

