#pragma once
#include <vector>
#include <string>
#ifdef _DEBUG
#include "Observer.h"
#endif // DEBUG

class CGraphManager;
struct ImFontAtlas;

namespace IronWroughtImGui {
	class CWindow;
}

class CImguiManager 
#ifdef _DEBUG
	: public IObserver
#endif
{
public:
	CImguiManager();
	~CImguiManager();
	void Init(CGraphManager* aGraphManager);
	void Update();
	void DebugWindow();

public://Inherited
#ifdef _DEBUG
	void Receive(const SMessage& aMessage) override;
#endif
private:

#ifdef _DEBUG
	std::vector<std::unique_ptr<IronWroughtImGui::CWindow>> myWindows;
#endif

	CGraphManager* myGraphManager;
	const std::string GetSystemMemory();
	const std::string GetDrawCalls();

	bool myGraphManagerIsFullscreen;
	bool myIsEnabled;
	std::string myScriptsStatus;
};
