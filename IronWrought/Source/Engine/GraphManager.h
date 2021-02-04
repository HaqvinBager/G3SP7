#pragma once

#include <imgui_node_editor.h>
#include <stack>

class CNodeInstance;

namespace ed = ax::NodeEditor;
class CGraphManager
{
public:
	~CGraphManager();
	void Load();

	void ReTriggerUpdateringTrees();
	
	void PreFrame(float aDeltaTime);
	void ConstructEditorTreeAndConnectLinks();
	void PostFrame();

	void ReTriggerTree();
	void SaveTreeToFile();
	void LoadTreeFromFile();
	void SaveNodesToClipboard();
	void LoadNodesFromClipboard();

	static void ShowFlow(int aLinkID);

	void Render();
	void PostRender();

	void ToggleShouldRenderGraph();
private:
	ImTextureID HeaderTextureID();
	void WillBeCyclic(CNodeInstance* aFirst, CNodeInstance* aSecond, bool& aIsCyclic, CNodeInstance* aBase);
	
	CNodeInstance* GetNodeFromPinID(unsigned int anID);
	CNodeInstance* GetNodeFromNodeID(unsigned int anID);
	void DrawTypeSpecificPin(struct SPin& aPin, CNodeInstance* aNodeInstance);
	std::vector<CNodeInstance*> myNodeInstancesInGraph;

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
	//ImTextureID myHeaderTextureID;
	ImVector<SEditorLinkInfo> myLinks;
	int myNextLinkIdCounter = 100;
	bool myLikeToSave = false;
	bool myLikeToShowFlow = false;
	char* myMenuSeachField = nullptr;
	bool mySetSearchFokus = true;
	bool myShouldPushCommand = true;
	ImTextureID myHeaderTextureID;
	bool myShouldRenderGraph;
};
