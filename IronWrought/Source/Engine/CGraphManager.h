#pragma once
//# include "Node Editor/imgui_node_editor.h"

#include <imgui_node_editor.h>
#include <stack>

namespace ed = ax::NodeEditor;
class CGraphManager
{
public:
	~CGraphManager();
	void Load();

	void ReTriggerUpdateringTrees();
	
	void PreFrame(float aTimeDelta);
	void ConstructEditorTreeAndConnectLinks();
	void Update();
	void PostFrame();

	void ReTriggerTree();
	void SaveTreeToFile();
	void LoadTreeFromFile();
	void SaveNodesToClipboard();
	void LoadNodesFromClipboard();

	static void ShowFlow(int aLinkID);
private:
	void WillBeCyclic(class CNodeInstance* aFirst, class CNodeInstance* aSecond, bool& aIsCyclic, class CNodeInstance* aBase);
	
	class CNodeInstance* GetNodeFromPinID(unsigned int aID);
	CNodeInstance* GetNodeFromNodeID(unsigned int aID);
	void DrawTypeSpecificPin(struct SPin& aPin, class CNodeInstance* aNodeInstance);
	std::vector<class CNodeInstance*> myNodeInstancesInGraph;

	struct EditorLinkInfo
	{
		ed::LinkId Id;
		ed::PinId  InputId;
		ed::PinId  OutputId;
	};

	enum class CommandAction {
		Create,
		Delete,
		AddLink,
		RemoveLink,
		ChangeDataOnPin,
		Count
	};

	struct EditorCommand
	{
		CommandAction myAction;
		CNodeInstance* myNodeInstance;
		CNodeInstance* mySecondNodeInstance;
		EditorLinkInfo myEditorLinkInfo;
		unsigned int myResourceUID;
		//unsigned int myInputPinID;
		//unsigned int myOutputPinID;

		//void Do();
		//void Undo();
	};

	std::stack<EditorCommand> myUndoCommands;
	std::stack<EditorCommand> myRedoCommands;

	ImVector<EditorLinkInfo> myLinks;
	int myNextLinkIdCounter = 100;
	bool myLikeToSave = false;
	bool myLikeToShowFlow = false;
	char* myMenuSeachField = nullptr;
	bool mySetSearchFokus = true;
	bool myShouldPushCommand = true;
};
