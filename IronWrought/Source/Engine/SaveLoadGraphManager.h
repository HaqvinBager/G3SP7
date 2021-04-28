#pragma once

class CGraphManager;
struct SPin;
class CJsonReader;

class CSaveLoadGraphManager
{
public:
	CSaveLoadGraphManager(CGraphManager* aGraphManager);
	~CSaveLoadGraphManager() { myGraphManager = nullptr; }
	void GraphManager(CGraphManager* aGraphManager) { myGraphManager = aGraphManager; }
	void LoadScripts(CGraphManager& aGraphManager, const std::string& aSceneName, std::string& aSceneFolder);
	void LoadTreeFromFile();
	void SaveTreeToFile();

	void SaveNodesToClipboard();
	void LoadNodesFromClipboard();

private:
	CGraphManager* myGraphManager;
};

