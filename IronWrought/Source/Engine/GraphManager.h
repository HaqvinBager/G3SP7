#pragma once
#include <stack>
#include <unordered_map>
#include <imgui_node_editor.h>

class CNodeInstance;
class CNodeType;
class CGameObject;
class CNodeGraphSaveLoad;
class CDrawNodePins;

namespace ed = ax::NodeEditor;

class CGraphManager
{
	friend class CNodeGraphSaveLoad;
private:
	enum class ECommandAction {
		ECreate,
		EDelete,
		EAddLink,
		ERemoveLink,
		EChangeDataOnPin,
		ECount
	};

	struct SEditorLinkInfo
	{
		ed::LinkId myID;

		ed::PinId  myInputID;
		ed::PinId  myOutputID;
	};

	struct SEditorCommand
	{
		ECommandAction myAction;
		
		CNodeInstance* myFirstNodeInstance;
		CNodeInstance* mySecondNodeInstance;
		
		SEditorLinkInfo myEditorLinkInfo;
		
		unsigned int myResourceUID;
	};

	struct BluePrintInstance {
		int rootID;
		std::vector<int> childrenIDs;
	};

public:
	struct SGraph
	{
		std::vector<CNodeInstance*> myNodeInstances;
		std::vector<BluePrintInstance> myBluePrintInstances;
		ImVector<SEditorLinkInfo> myLinks;

		std::string myFolderPath;
		std::string myChildrenKey;

		unsigned int myNextLinkIdCounter = 100;

		void Clear();
	};
	
public:
	inline void CurrentGraph(SGraph* aGraph) { myCurrentGraph = aGraph; }
	inline SGraph CurrentGraph() const { return *myCurrentGraph; }
	
	inline std::vector<SGraph> Graphs() const { return myGraphs; }
	inline void AddGraph(const SGraph& aGraph) { myGraphs.push_back(aGraph); }
	inline SGraph Graph(const unsigned int& anIndex) { return myGraphs[anIndex]; }
	inline void Graph(SGraph& aGraph, const unsigned int& anIndex) { myGraphs[anIndex] = aGraph; }
	
	inline const std::string SceneFolderPath() const { return mySceneFolder; }
	
	inline const int GetCurrentBlueprintInstanceID() const { return myCurrentBluePrintInstance.childrenIDs[0]; }
	
	inline const bool RunScripts() const { return myRunScripts; }
	inline const bool ToggleShouldRunScripts() { myRunScripts = !myRunScripts; return myRunScripts; }
	
	inline const std::string NewVariableType() const { return myNewVariableType; }
	inline void NewVariableType(const std::string& aNewVariableType) { myNewVariableType = aNewVariableType; }
	
	inline void AddCustomDataNode(const std::string& aCustomDataNode) { myCustomDataNodes.push_back(aCustomDataNode); }
	
	inline const bool ShouldRenderGraph() { return myRenderGraph; }
	inline void ToggleShouldRenderGraph() { myRenderGraph = !myRenderGraph; }
	
	inline void ShowFlow(const int& aLinkID) { if (aLinkID == 0) return; myFlowsToBeShown.push_back(aLinkID); }

	~CGraphManager();

	void Load(const std::string& aSceneName);
	void Clear();

	CGameObject* GetCurrentGameObject();
	std::vector<CGameObject*> GetCurrentGameObjectChildren();

	CNodeInstance* GetNodeFromNodeID(unsigned int anID);
	CNodeInstance* GetNodeFromPinID(unsigned int anID);

	void Update();

	void PreFrame();
	void CreateNewNode();
	void TriggerNodeGraphs();
	SEditorCommand CreateInverseEditorCommand(SEditorCommand& anEditorCommand);
	void DeleteNodeType(CNodeInstance& aNodeInstance);

	void ConstructEditorTreeAndConnectLinks();
	void AddNodeLinks();
	void WillBeCyclic(CNodeInstance* aFirst, bool& aIsCyclic, CNodeInstance* aBase);
	void DeleteLinksOrNodes();
	void CreateNewDataNode();
	void PopulateNodeList(std::vector<CNodeType*>& aNodeListToFill, CNodeType**& aNodeTypeList, const unsigned int& aNumberOfNodes);
	void UndoRedoActions();

	void PostFrame();

private:
	SGraph* myCurrentGraph;
	BluePrintInstance myCurrentBluePrintInstance;
	CNodeGraphSaveLoad* mySaveLoadGraphManager;
	CDrawNodePins* myPinDrawer;

	char* myMenuSearchField = nullptr;

	bool myRenderGraph;
	bool myRunScripts;
	bool myEnteringNodeName = false;
	bool mySetPosition = false;
	bool mySave = false;
	bool myShowFlow = false;
	bool mySearchFokus = true;
	bool myPushCommand = true;
	
	std::string mySceneFolder;
	std::string myNewVariableType;
	
	std::vector<std::string> myInstantiableVariables;
	std::vector<std::string> myCustomDataNodes;

	std::vector<SGraph> myGraphs;

	std::vector<int> myFlowsToBeShown;

	std::stack<SEditorCommand> myUndoCommands;
	std::stack<SEditorCommand> myRedoCommands;
};