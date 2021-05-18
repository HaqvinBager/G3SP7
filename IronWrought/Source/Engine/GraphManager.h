#pragma once
#ifdef _DEBUG
#include <stack>
#include <imgui_node_editor.h>
#endif // _DEBUG

class CNodeInstance;
class CNodeType;
class CGameObject;
class CNodeGraphSaveLoad;

#ifdef _DEBUG
class CDrawNodePins;
namespace ed = ax::NodeEditor;
#endif // _DEBUG

class CGraphManager
{
	friend class CNodeGraphSaveLoad;
private:
#ifdef _DEBUG
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
#endif // _DEBUG

	struct BluePrintInstance {
		int rootID;
		std::vector<int> childrenIDs;
	};

public:
	struct SGraph
	{
		std::vector<CNodeInstance*> myNodeInstances;
		std::vector<BluePrintInstance> myBluePrintInstances;

#ifdef _DEBUG
		ImVector<SEditorLinkInfo> myLinks;
#endif // _DEBUG

		std::string myFolderPath;
		std::string myChildrenKey;

		unsigned int myNextLinkIdCounter = 100;

		void Clear();
	};
	
public:
	~CGraphManager();

	inline void CurrentGraph(SGraph* aGraph) { myCurrentGraph = aGraph; }
	inline SGraph CurrentGraph() const { return *myCurrentGraph; }
	
	inline std::vector<SGraph> Graphs() const { return myGraphs; }
	inline void AddGraph(const SGraph& aGraph) { myGraphs.push_back(aGraph); }
	inline SGraph Graph(const unsigned int& anIndex) { return myGraphs[anIndex]; }
	inline void Graph(SGraph& aGraph, const unsigned int& anIndex) { myGraphs[anIndex] = aGraph; }
	
	inline const std::string SceneFolderPath() const { return mySceneFolder; }
	
	inline const int GetCurrentBlueprintInstanceID() const { return myCurrentBluePrintInstance.childrenIDs[0]; }
	
	inline const bool RunScripts() const { return myRunScripts; }
#ifdef _DEBUG
	inline const bool ToggleShouldRunScripts() { myRunScripts = !myRunScripts; return myRunScripts; }
#endif // _DEBUG
	
	inline const std::string NewVariableType() const { return myNewVariableType; }
	inline void NewVariableType(const std::string& aNewVariableType) { myNewVariableType = aNewVariableType; }
	
	inline void AddCustomDataNode(const std::string& aCustomDataNode) { myCustomDataNodes.push_back(aCustomDataNode); }
	
#ifdef _DEBUG
	inline const bool ShouldRenderGraph() { return myRenderGraph; }
	inline void ToggleShouldRenderGraph() { myRenderGraph = !myRenderGraph; }
	
	inline void ShowFlow(const int& aLinkID) { if (aLinkID == 0) return; myFlowsToBeShown.push_back(aLinkID); }
#endif // _DEBUG

	void Load(const std::string& aSceneName);
	void Clear();

	CGameObject* GetCurrentGameObject();
	std::vector<CGameObject*> GetCurrentGameObjectChildren();

	CNodeInstance* GetNodeFromNodeID(unsigned int anID);
	CNodeInstance* GetNodeFromPinID(unsigned int anID);

	void Update();

	void PreFrame();
#ifdef _DEBUG
	void CreateNewNode();
#endif // _DEBUG
	void TriggerNodeGraphs();
#ifdef _DEBUG
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
#endif // _DEBUG

private:
	SGraph* myCurrentGraph;
	BluePrintInstance myCurrentBluePrintInstance;
	CNodeGraphSaveLoad* mySaveLoadGraphManager;

#ifdef _DEBUG
	CDrawNodePins* myPinDrawer;

	char* myMenuSearchField = nullptr;
	bool myRenderGraph;
#endif // _DEBUG
	bool myRunScripts;
	
#ifdef _DEBUG
	bool myEnteringNodeName = false;
	bool mySetPosition = false;
	bool mySave = false;
	bool myShowFlow = false;
	bool mySearchFokus = true;
	bool myPushCommand = true;
#endif // _DEBUG
	
	std::string mySceneFolder;
	std::string myNewVariableType;
	
	std::vector<std::string> myInstantiableVariables;
	std::vector<std::string> myCustomDataNodes;

	std::vector<SGraph> myGraphs;

#ifdef _DEBUG
	std::vector<int> myFlowsToBeShown;

	std::stack<SEditorCommand> myUndoCommands;
	std::stack<SEditorCommand> myRedoCommands;
#endif // _DEBUG
};