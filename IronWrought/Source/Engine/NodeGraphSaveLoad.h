#pragma once

class CGraphManager;
struct SPin;
class CJsonReader;

class CNodeGraphSaveLoad
{
public:
	void LoadScripts(CGraphManager& aGraphManager, const std::string& aSceneName, std::string& aSceneFolder);
	void LoadTreeFromFile(CGraphManager& aGraphManager);
#ifdef _DEBUG
	void SaveTreeToFile(CGraphManager& aGraphManager);

	void SaveNodesToClipboard(CGraphManager& aGraphManager);
	void LoadNodesFromClipboard(CGraphManager& aGraphManager);
	void LoadDataNodesFromFile(const std::string& aSceneFolder, std::string& aNewVariableType);
#endif // _DEBUG

private:
};

