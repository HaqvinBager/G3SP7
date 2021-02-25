#pragma once
#include <vector>
#include <string>

class CGraphManager;
struct ImFontAtlas;

namespace ImGuiWindow {
	class CWindow;
}

class CImguiManager
{
public:
	CImguiManager();
	~CImguiManager();
	void Update();
	void DebugWindow();

private:
	std::vector<std::unique_ptr<ImGuiWindow::CWindow>> myWindows;

	CGraphManager* myGraphManager;
	const std::string GetSystemMemory();
	const std::string GetDrawCalls();

	bool myGraphManagerIsFullscreen;
	bool myIsEnabled;
	std::string myScriptsStatus;
};
