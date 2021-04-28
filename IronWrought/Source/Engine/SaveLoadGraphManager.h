#pragma once

class CGraphManager;
struct SPin;
class CJsonReader;

class CSaveLoadGraphManager
{
public:
	CSaveLoadGraphManager() : myGraphManager(nullptr){}
	~CSaveLoadGraphManager() { myGraphManager = nullptr; }
	void GraphManager(CGraphManager& aGraphManager) { myGraphManager = &aGraphManager; }
	void LoadScripts(const std::string& aSceneName, std::string& aSceneFolder);
	void LoadTreeFromFile();
	void SaveTreeToFile();

	void SaveNodesToClipboard();
	void LoadNodesFromClipboard();

private:
	CGraphManager* myGraphManager;
};

