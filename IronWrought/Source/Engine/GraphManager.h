#pragma once

#include <imgui_node_editor.h>
#include <stack>
#include <unordered_map>
//#include "NodeData.h"

class CNodeInstance;
class CGameObject;

namespace ed = ax::NodeEditor;
class CGraphManager
{
public:
	~CGraphManager();
	void Load();

	void ReTriggerUpdatingTrees();
	
	void PreFrame(float aDeltaTime);
	void ConstructEditorTreeAndConnectLinks();
	void PostFrame();

	//void ReTriggerTree();
	void SaveTreeToFile();
	void LoadTreeFromFile();
	void SaveNodesToClipboard();
	void LoadNodesFromClipboard();

	void ShowFlow(int aLinkID);


	void Update();

	void ToggleShouldRenderGraph();
	bool ToggleShouldRunScripts();
	bool ShouldRenderGraph() { return myShouldRenderGraph; }

	CGameObject* GetCurrentGameObject();

private:
	ImTextureID HeaderTextureID();
	void WillBeCyclic(CNodeInstance* aFirst, CNodeInstance* aSecond, bool& aIsCyclic, CNodeInstance* aBase);
	
	CNodeInstance* GetNodeFromPinID(unsigned int anID);
	CNodeInstance* GetNodeFromNodeID(unsigned int anID);
	void DrawTypeSpecificPin(struct SPin& aPin, CNodeInstance* aNodeInstance);

	void CreateNewDataNode();
	void LoadDataNodesFromFile();

	std::unordered_map<std::string, std::vector<CNodeInstance*>> myGraphs;
	std::unordered_map<std::string, std::vector<int>> myGameObjectIDsMap;
	std::vector<std::string> myKeys;
	std::vector<int> myFlowsToBeShown;

	struct SEditorLinkInfo
	{
		ed::LinkId myID;
		ed::PinId  myInputID;
		ed::PinId  myOutputID;
	};

	enum class ECommandAction {
		ECreate,
		EDelete,
		EAddLink,
		ERemoveLink,
		EChangeDataOnPin,
		ECount
	};

	struct EditorCommand
	{
		ECommandAction myAction;
		CNodeInstance* myNodeInstance;
		CNodeInstance* mySecondNodeInstance;
		SEditorLinkInfo myEditorLinkInfo;
		unsigned int myResourceUID;
		//unsigned int myInputPinID;
		//unsigned int myOutputPinID;

		//void Do();
		//void Undo();
	};

	std::stack<EditorCommand> myUndoCommands;
	std::stack<EditorCommand> myRedoCommands;
	ImVector<SEditorLinkInfo> myLinks;
	std::vector<std::string> myInstantiableVariables;
	//std::unordered_map<std::string, CNodeData*> myVariableNodeDataMap;
	bool myIsEnteringNodeName = false;
	bool myHasSetPosition = false;
	std::string myNewVariableType;
	int myNextLinkIdCounter = 100;
	bool myLikeToSave = false;
	bool myLikeToShowFlow = false;
	char* myMenuSeachField = nullptr;
	bool mySetSearchFokus = true;
	bool myShouldPushCommand = true;
	ImTextureID myHeaderTextureID;
	bool myShouldRenderGraph;
	bool myScriptShouldRun;
	std::string myCurrentPath;
	std::string myCurrentKey;
	int myCurrentGameObjectID;
};
