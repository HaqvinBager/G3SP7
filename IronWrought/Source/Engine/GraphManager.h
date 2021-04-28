#pragma once

#include <stack>
#include <unordered_map>

class CNodeInstance;
class CNodeType;
class CGameObject;
class CSaveLoadGraphManager;
class CDrawGraphManager;
struct SPin;

#ifdef _DEBUG
#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;
#endif // DEBUG

class CGraphManager
{
	friend class CSaveLoadGraphManager;
public:
	bool RunScripts() { return myRunScripts; }
#ifdef _DEBUG
	~CGraphManager();
	void ShowFlow(int aLinkID) { if (aLinkID == 0) return; myFlowsToBeShown.push_back(aLinkID); }
	void ToggleShouldRenderGraph() { myRenderGraph = !myRenderGraph; }
	bool ToggleShouldRunScripts() { myRunScripts = !myRunScripts; return myRunScripts; }
	bool ShouldRenderGraph() { return myRenderGraph; }
	
	void PostFrame();
	void ConstructEditorTreeAndConnectLinks();
#endif // DEBUG
	
	void Load(const std::string& aSceneName);
	void Clear();
	void ReTriggerUpdatingTrees();
	void PreFrame();
	void Update();
	CGameObject* GetCurrentGameObject();
	std::vector<CGameObject*> GetCurrentGameObjectChildren();
	const int GetCurrentBlueprintInstanceID() const { return myCurrentBluePrintInstance.childrenIDs[0]; }

private:
	enum class ECommandAction {
		ECreate,
		EDelete,
		EAddLink,
		ERemoveLink,
		EChangeDataOnPin,
		ECount
	};

#ifdef _DEBUG
	struct SEditorLinkInfo
	{
		ed::LinkId myID;
		ed::PinId  myInputID;
		ed::PinId  myOutputID;
	};

	struct EditorCommand
	{
		ECommandAction myAction;
		CNodeInstance* myNodeInstance;
		CNodeInstance* mySecondNodeInstance;
		SEditorLinkInfo myEditorLinkInfo;
		unsigned int myResourceUID;
	};
#endif	

	struct BluePrintInstance {
		int rootID;
		std::vector<int> childrenIDs;
	};

	struct SGraph
	{
		std::vector<CNodeInstance*> myNodeInstances;
		std::vector<BluePrintInstance> myBluePrintInstances;
#ifdef _DEBUG
		ImVector<SEditorLinkInfo> myLinks;
#endif
		std::string myFolderPath;
		std::string myChildrenKey;
		unsigned int myNextLinkIdCounter = 100;

		void Clear();
	};
public:
	std::vector<SGraph> Graphs() const { return myGraphs; }
	void CurrentGraph(SGraph* aGraph);
	void Graph(SGraph aGraph);
	SGraph CurrentGraph() const { return *myCurrentGraph; }

private:
	void RegisterNewDataNode(std::string aName);
#ifdef _DEBUG
	EditorCommand CreateInverseEditorCommand(EditorCommand &anEditorCommand);
	void DeleteNodeType(CNodeInstance& aNodeInstance);
	void CreateNewDataNode();
	void PopulateNodeList(std::vector<CNodeType*>& aNodeListToFill, CNodeType**& aNodeTypeList, const unsigned int& aNumberOfNodes);

	ImTextureID HeaderTextureID();
#endif // _DEBUG
	void WillBeCyclic(CNodeInstance* aFirst, bool& aIsCyclic, CNodeInstance* aBase);
	void LoadDataNodesFromFile();

public:
	CNodeInstance* GetNodeFromPinID(unsigned int anID);
	CNodeInstance* GetNodeFromNodeID(unsigned int anID);

private:
	SGraph* myCurrentGraph;
	BluePrintInstance myCurrentBluePrintInstance;
	CSaveLoadGraphManager* mySaveLoadGraphManager;
	CDrawGraphManager* myDrawGraphManager;

	char* myMenuSearchField = nullptr;
	bool myEnteringNodeName = false;
	bool mySetPosition = false;
	bool mySave = false;
	bool myShowFlow = false;
	bool mySearchFokus = true;
	bool myPushCommand = true;
	bool myRenderGraph;
	bool myRunScripts;
	
	std::string mySceneFolder;
	std::string myNewVariableType;
	
	std::vector<std::string> myInstantiableVariables;
	std::vector<std::string> myCustomDataNodes;
	std::vector<SGraph> myGraphs;

#ifdef _DEBUG
	std::vector<int> myFlowsToBeShown;
	std::stack<EditorCommand> myUndoCommands;
	std::stack<EditorCommand> myRedoCommands;
	ImTextureID myHeaderTextureID;
#endif
};
