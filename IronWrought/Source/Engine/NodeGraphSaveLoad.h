#pragma once

class CGraphManager;
struct SPin;
class CJsonReader;

class CNodeGraphSaveLoad
{
public:
	void LoadScripts(CGraphManager& aGraphManager, const std::string& aSceneName, std::string& aSceneFolder);
	void LoadTreeFromFile(CGraphManager& aGraphManager);
	void LoadDataNodesFromFile(CGraphManager& aGraphManager);
	void SaveTreeToFile(CGraphManager& aGraphManager);
	void SaveNodesToClipboard(CGraphManager& aGraphManager);
	void LoadNodesFromClipboard(CGraphManager& aGraphManager);

private:
};

