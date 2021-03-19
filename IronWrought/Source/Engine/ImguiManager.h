#pragma once
#include <vector>
#include <string>

#include "Observer.h"

class CGraphManager;
struct ImFontAtlas;

namespace ImGuiWindow {
	class CWindow;
}
//namespace IronWroughtImGui {
//	class CWindow;
//}

class CImguiManager : public IObserver
{
public:
	CImguiManager();
	~CImguiManager();
	void Update();
	void DebugWindow();

public://Inherited
	void Receive(const SMessage& aMessage) override;

private:
	std::vector<std::unique_ptr<ImGuiWindow::CWindow>> myWindows;

	CGraphManager* myGraphManager;
	const std::string GetSystemMemory();
	const std::string GetDrawCalls();

	bool myGraphManagerIsFullscreen;
	bool myIsEnabled;
	std::string myScriptsStatus;
};
