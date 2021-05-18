#include "stdafx.h"
#include "GraphManager.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"
#include "JsonReader.h"
#include <fstream>
#include <sstream>
#include "NodeInstance.h"
#include "NodeType.h"
#include "NodeTypes.h"
#include "Input.h"
#include <filesystem>
#include "Scene.h"
#include "Engine.h"
#include "GameObject.h"
#include "GraphNodeTimerManager.h"
#include "FolderUtility.h"
#include "NodeDataManager.h"
#include "NodeGraphSaveLoad.h"
#ifdef _DEBUG
#include "DrawNodePins.h"
#include <imgui_node_editor.h>
#include "Drawing.h"
#include "Widgets.h"
#include "Interop.h"
#include <imgui_impl_dx11.h>

using namespace ax::Drawing;
static ed::EditorContext* g_Context = nullptr;
#endif // _DEBUG
using namespace rapidjson;

CGraphManager::~CGraphManager()
{
	Clear();
#ifdef _DEBUG
	ed::DestroyEditor(g_Context);
#endif // _DEBUG
}

void CGraphManager::SGraph::Clear()
{
	for (size_t i = 0; i < myNodeInstances.size(); ++i)
	{
		delete myNodeInstances[i];
		myNodeInstances[i] = nullptr;
	}
	myNodeInstances.clear();
	myBluePrintInstances.clear();
}

void CGraphManager::Load(const std::string& aSceneName)
{
#ifdef _DEBUG
	myPinDrawer = new CDrawNodePins();
	myPinDrawer->GraphManager(*this);
#endif // _DEBUG
	mySaveLoadGraphManager = new CNodeGraphSaveLoad();

#ifdef _DEBUG
	ed::Config config;
	config.SettingsFile = "Imgui/NodeScripts/Simple.json";
	g_Context = ed::CreateEditor(&config);
	myRunScripts = false;
	myRenderGraph = false;
	myMenuSearchField = new char[127];
	memset(&myMenuSearchField[0], 0, sizeof(myMenuSearchField));
#else
	myRunScripts = true;
#endif // _DEBUG

	if (myGraphs.size() > 0)
		myCurrentGraph = &myGraphs[0];

	CGraphNodeTimerManager::Create();
	CNodeDataManager::Get()->SetFolderPath(mySceneFolder);
	mySaveLoadGraphManager->LoadScripts(*this, aSceneName, mySceneFolder);
	CNodeTypeCollector::PopulateTypes();
	mySaveLoadGraphManager->LoadDataNodesFromFile(*this);
	mySaveLoadGraphManager->LoadTreeFromFile(*this);

	if (myGraphs.size() > 0)
		myCurrentGraph = &myGraphs[0];

	if (!CNodeDataManager::Get())
	{
		CNodeDataManager::Create();
		myInstantiableVariables.push_back("Float");
		myInstantiableVariables.push_back("Int");
		myInstantiableVariables.push_back("Bool");
		myInstantiableVariables.push_back("Start");
		myInstantiableVariables.push_back("Vector 3");
	}
}

void CGraphManager::Clear()
{
	if (myGraphs.size() <= 0)
		return;

	mySaveLoadGraphManager->SaveTreeToFile(*this);

	CUID::ClearUIDS();

	for (auto& sGraph : myGraphs)
		sGraph.Clear();

	myGraphs.clear();
	myCurrentGraph = nullptr;

	CNodeDataManager::Get()->ClearStoredData();
}

CGameObject* CGraphManager::GetCurrentGameObject()
{
	CScene& scene = CEngine::GetInstance()->GetActiveScene();

	CGameObject* gameObject = scene.FindObjectWithID(myCurrentBluePrintInstance.childrenIDs[0]);

	if (!gameObject)
		myRunScripts = false;

	return gameObject;
}

std::vector<CGameObject*> CGraphManager::GetCurrentGameObjectChildren()
{
	CScene& scene = CEngine::GetInstance()->GetActiveScene();
	std::vector<CGameObject*> gameObjects = {};
	for (int i = 1; i < myCurrentBluePrintInstance.childrenIDs.size(); ++i)
	{
		gameObjects.push_back(scene.FindObjectWithID(myCurrentBluePrintInstance.childrenIDs[i]));
		if (!gameObjects.back())
			myRunScripts = false;
	}

	return gameObjects;
}

CNodeInstance* CGraphManager::GetNodeFromNodeID(unsigned int anID)
{
	if (anID == 0)
		return nullptr;

	auto it = myCurrentGraph->myNodeInstances.begin();
	while (it != myCurrentGraph->myNodeInstances.end())
	{
		if ((*it)->UID().AsInt() == anID)
			return *it;
		else
			++it;
	}

	return nullptr;
}

CNodeInstance* CGraphManager::GetNodeFromPinID(unsigned int anID)
{
	if (anID == 0)
		return nullptr;

	for (auto& nodeInstance : myCurrentGraph->myNodeInstances)
	{
		std::vector<SPin>& pins = nodeInstance->GetPins();

		for (auto& pin : pins)
		{
			if (pin.myUID.AsInt() == anID)
				return nodeInstance;
		}
	}
	return nullptr;
}

void CGraphManager::Update()
{
	if (myGraphs.size() > 0)
	{
		CGraphNodeTimerManager::Get()->Update();

		PreFrame();

#ifdef _DEBUG
		if (myRenderGraph)
		{
			ConstructEditorTreeAndConnectLinks();
			PostFrame();
			ImGui::End();
		}
#endif // _DEBUG
	}
}

void CGraphManager::PreFrame()
{
#ifdef _DEBUG
	if (myRenderGraph)
	{
		auto& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(0, 18));
		ImGui::SetNextWindowSize({ io.DisplaySize.x,  io.DisplaySize.y });
		ImGui::SetNextWindowBgAlpha(0.5f);

		ImGui::Begin(myCurrentGraph->myFolderPath.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

		ImGui::SameLine();
		if (ImGui::Button("Save"))
			mySave = true;

		ImGui::SetNextWindowPos(ImVec2(8.5f, 69.5f));
		ImGui::SetNextWindowSize({ 200.f,  io.DisplaySize.y - 69.5f });

		ImGui::Begin("Scripts:", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		static int selected = -1;

		for (int i = 0; i < myGraphs.size(); i++)
		{
			char buf[512];
			sprintf_s(buf, "%s", myGraphs[i].myFolderPath.c_str());

			if (ImGui::Selectable(buf, selected == i, ImGuiSelectableFlags_AllowDoubleClick))
			{
				selected = i;

				if (ImGui::IsMouseClicked(0))
					myCurrentGraph = &myGraphs[i];
			}
		}
		ImGui::End();
	}
	CreateNewDataNode();

	for (auto& nodeInstance : myCurrentGraph->myNodeInstances)
	{
		nodeInstance->DebugUpdate();
		nodeInstance->VisualUpdate(CTimer::Dt());
	}
#endif // _DEBUG

	TriggerNodeGraphs();

#ifdef _DEBUG
	if (myRenderGraph)
	{
		ed::SetCurrentEditor(g_Context);
		ed::Begin("My Editor", ImVec2(0.0, 0.0f));
	}
#endif // _DEBUG
}

#ifdef _DEBUG
void CGraphManager::CreateNewDataNode()
{
	if (myEnteringNodeName)
	{
		if (!mySetPosition)
		{
			ImGui::SetNextWindowPos({ ImGui::GetIO().MousePos.x,ImGui::GetIO().MousePos.y });
			mySetPosition = true;
		}

		ImGui::SetNextWindowSize({ 225, 60 });
		ImGui::Begin(myNewVariableType.c_str());
		static char buffer[64] = "";
		ImGui::InputText("Name", buffer, 64);

		if (Input::GetInstance()->IsKeyPressed(VK_RETURN))
		{
			mySetPosition = false;
			myEnteringNodeName = false;
			bool hasCreatedNewVariable = false;
			if (strlen(buffer) == 0)
				memcpy(buffer, myNewVariableType.c_str(), 10);

			for (unsigned int i = 0; i < CNodeTypeCollector::GetNodeTypeCount(CNodeType::ENodeType::EChild); ++i)
			{
				if (CNodeTypeCollector::GetAllNodeTypes(CNodeType::ENodeType::EChild)[i]->NodeName() == buffer)
				{
					hasCreatedNewVariable = true;
					break;
				}
			}
			if (!hasCreatedNewVariable)
			{
				CNodeDataManager::Get()->RegisterNewDataNode(buffer, *this);
				CNodeDataManager::Get()->SaveDataTypesToJson();
				ZeroMemory(buffer, 64);
				hasCreatedNewVariable = false;
			}
		}
		ImGui::End();
	}
}
#endif // _DEBUG

void CGraphManager::TriggerNodeGraphs()
{
	if (myRunScripts)
	{
		for (const auto& graph : myGraphs)
		{
			const auto& bluePrintInstances = graph.myBluePrintInstances;

			for (unsigned int i = 0; i < bluePrintInstances.size(); ++i)
			{
				myCurrentBluePrintInstance = bluePrintInstances[i];
				for (auto& nodeInstance : graph.myNodeInstances)
				{
					if (nodeInstance->NodeType()->IsStartNode())
						nodeInstance->Enter();
				}
			}
		}
	}
}

#ifdef _DEBUG
CGraphManager::SEditorCommand CGraphManager::CreateInverseEditorCommand(SEditorCommand& anEditorCommand)
{
	SEditorCommand inverseCommand = anEditorCommand;
	SPin* firstPin;
	SPin* secondPin;

	switch (anEditorCommand.myAction)
	{
	case CGraphManager::ECommandAction::ECreate:
	{
		inverseCommand.myAction = ECommandAction::EDelete;
		ed::DeleteNode(anEditorCommand.myResourceUID);
	}
	break;
	case CGraphManager::ECommandAction::EDelete:
	{
		inverseCommand.myAction = ECommandAction::ECreate;
		myCurrentGraph->myNodeInstances.push_back(anEditorCommand.myFirstNodeInstance);
	}
	break;
	case CGraphManager::ECommandAction::EAddLink:
	{
		inverseCommand.myAction = ECommandAction::ERemoveLink;
		ed::DeleteLink(anEditorCommand.myEditorLinkInfo.myID);
	}
	break;
	case CGraphManager::ECommandAction::ERemoveLink:
	{
		inverseCommand.myAction = ECommandAction::EAddLink;
		anEditorCommand.myFirstNodeInstance->AddLinkToVia(anEditorCommand.mySecondNodeInstance, static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myInputID.Get()), static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myOutputID.Get()), anEditorCommand.myResourceUID);
		anEditorCommand.mySecondNodeInstance->AddLinkToVia(anEditorCommand.myFirstNodeInstance, static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myOutputID.Get()), static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myInputID.Get()), anEditorCommand.myResourceUID);

		firstPin = anEditorCommand.myFirstNodeInstance->GetPinFromID(static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myInputID.Get()));
		secondPin = anEditorCommand.mySecondNodeInstance->GetPinFromID(static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myOutputID.Get()));

		if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
			myCurrentGraph->myLinks.push_back({ anEditorCommand.myEditorLinkInfo.myID, anEditorCommand.myEditorLinkInfo.myInputID, anEditorCommand.myEditorLinkInfo.myOutputID });
		else
			myCurrentGraph->myLinks.push_back({ anEditorCommand.myEditorLinkInfo.myID, anEditorCommand.myEditorLinkInfo.myOutputID, anEditorCommand.myEditorLinkInfo.myInputID });
	}
	break;
	default:
		break;
	}
	return inverseCommand;
}

void CGraphManager::DeleteNodeType(CNodeInstance& aNodeInstance)
{
	aNodeInstance.NodeType()->ClearNodeInstanceFromMap(&aNodeInstance);
	std::vector<SNodeInstanceLink> links = (&aNodeInstance)->GetLinks();

	for (auto& link : links)
	{
		CNodeInstance* firstNode = GetNodeFromPinID(static_cast<unsigned int>(link.myFromPinID));
		CNodeInstance* secondNode = GetNodeFromPinID(static_cast<unsigned int>(link.myToPinID));

		firstNode->RemoveLinkToVia(secondNode, static_cast<unsigned int>(link.myFromPinID));
		secondNode->RemoveLinkToVia(firstNode, static_cast<unsigned int>(link.myToPinID));
	}
}

void CGraphManager::ConstructEditorTreeAndConnectLinks()
{
	myPinDrawer->DrawNodes();

	if (!myRunScripts)
	{
		AddNodeLinks();
		DeleteLinksOrNodes();

		CreateNewNode();

		myPushCommand = true;

		UndoRedoActions();
	}
}
#endif // _DEBUG

bool ArePinTypesCompatible(SPin& aFirst, SPin& aSecond)
{
	if ((aFirst.myVariableType == SPin::EPinType::EFlow && aSecond.myVariableType != SPin::EPinType::EFlow))
		return false;

	if ((aSecond.myVariableType == SPin::EPinType::EFlow && aFirst.myVariableType != SPin::EPinType::EFlow))
		return false;

	return true;
}

#ifdef _DEBUG
void CGraphManager::AddNodeLinks()
{
	if (ed::BeginCreate())
	{
		ed::PinId inputPinId, outputPinId;
		if (ed::QueryNewLink(&inputPinId, &outputPinId))
		{
			if (inputPinId && outputPinId)
			{
				if (ed::AcceptNewItem())
				{
					unsigned int inputPinID = static_cast<unsigned int>(inputPinId.Get());
					unsigned int outputPinID = static_cast<unsigned int>(outputPinId.Get());
					CNodeInstance* firstNode = GetNodeFromPinID(inputPinID);
					CNodeInstance* secondNode = GetNodeFromPinID(outputPinID);
					assert(firstNode);
					assert(secondNode);

					if (firstNode != secondNode)
					{
						SPin* firstPin = firstNode->GetPinFromID(inputPinID);
						SPin* secondPin = secondNode->GetPinFromID(outputPinID);

						bool canAddlink = true;
						if (firstPin && secondPin)
						{
							if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN && secondPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
								canAddlink = false;

							if (!ArePinTypesCompatible(*firstPin, *secondPin))
								canAddlink = false;

							if (!firstNode->CanAddLink(inputPinID))
								canAddlink = false;

							if (!secondNode->CanAddLink(outputPinID))
								canAddlink = false;

							if (firstNode->HasLinkBetween(inputPinID, outputPinID))
								canAddlink = false;

							if (canAddlink)
							{
								if (secondPin->myVariableType == SPin::EPinType::EUnknown)
									secondNode->ChangePinTypes(firstPin->myVariableType);

								unsigned int linkId = ++myCurrentGraph->myNextLinkIdCounter;
								firstNode->AddLinkToVia(secondNode, inputPinID, outputPinID, linkId);
								secondNode->AddLinkToVia(firstNode, outputPinID, inputPinID, linkId);

								bool aIsCyclic = false;
								WillBeCyclic(firstNode, aIsCyclic, firstNode);
								if (aIsCyclic || !canAddlink)
								{
									firstNode->RemoveLinkToVia(secondNode, inputPinID);
									secondNode->RemoveLinkToVia(firstNode, outputPinID);
								}
								else
								{
									if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
										myCurrentGraph->myLinks.push_back({ ed::LinkId(linkId), outputPinId, inputPinId });
									else
										myCurrentGraph->myLinks.push_back({ ed::LinkId(linkId), inputPinId, outputPinId });

									myUndoCommands.push({ ECommandAction::EAddLink, firstNode, secondNode, myCurrentGraph->myLinks.back(), 0 });

									mySave = true;
								}
							}
						}
					}
				}
			}
		}
	}
	ed::EndCreate();
}

void CGraphManager::WillBeCyclic(CNodeInstance* aFirst, bool& aIsCyclic, CNodeInstance* aBase)
{
	if (aFirst == nullptr)
		return;

	if (aBase == nullptr)
		return;

	if (aIsCyclic)
		return;

	std::vector<SPin>& pins = aFirst->GetPins();
	for (auto& pin : pins)
	{
		if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_OUT)
		{
			std::vector< SNodeInstanceLink*> links = aFirst->GetLinkFromPin(pin.myUID.AsInt());
			if (links.size() == 0)
				return;

			for (auto& link : links)
			{
				CNodeInstance* connectedNodeToOutPut = GetNodeFromPinID(link->myToPinID);
				if (connectedNodeToOutPut == aBase)
				{
					aIsCyclic |= true;
					return;
				}
				WillBeCyclic(connectedNodeToOutPut, aIsCyclic, aBase);
			}
		}
	}
}

void CGraphManager::DeleteLinksOrNodes()
{
	if (ed::BeginDelete())
	{
		ed::LinkId deletedLinkId;
		while (ed::QueryDeletedLink(&deletedLinkId))
		{
			if (ed::AcceptDeletedItem())
			{
				for (auto& link : myCurrentGraph->myLinks)
				{
					if (link.myID == deletedLinkId)
					{
						CNodeInstance* firstNode = GetNodeFromPinID(static_cast<unsigned int>(link.myInputID.Get()));
						CNodeInstance* secondNode = GetNodeFromPinID(static_cast<unsigned int>(link.myOutputID.Get()));
						assert(firstNode);
						assert(secondNode);

						firstNode->RemoveLinkToVia(secondNode, static_cast<unsigned int>(link.myInputID.Get()));
						secondNode->RemoveLinkToVia(firstNode, static_cast<unsigned int>(link.myOutputID.Get()));

						if (myPushCommand)
							myUndoCommands.push({ ECommandAction::ERemoveLink, firstNode, secondNode, link, 0 });

						myCurrentGraph->myLinks.erase(&link);
						mySave = true;

						break;
					}
				}
			}
		}
		ed::NodeId nodeId = 0;
		while (ed::QueryDeletedNode(&nodeId))
		{
			if (ed::AcceptDeletedItem())
			{
				auto it = myCurrentGraph->myNodeInstances.begin();
				while (it != myCurrentGraph->myNodeInstances.end())
				{
					if ((*it)->UID().AsInt() == nodeId.Get())
					{
						(*it)->NodeType()->ClearNodeInstanceFromMap((*it));
						if (myPushCommand)
						{
							mySave = true;
							myUndoCommands.push({ ECommandAction::EDelete, (*it), nullptr,  {0,0,0}, (*it)->UID().AsInt() });
						}
						it = myCurrentGraph->myNodeInstances.erase(it);
					}
					else
						++it;
				}
			}
		}
	}
	ed::EndDelete();
}

void CGraphManager::CreateNewNode()
{
	auto openPopupPosition = ImGui::GetMousePos();
	ed::Suspend();

	if (ed::ShowBackgroundContextMenu())
		ImGui::OpenPopup("Create New Node");

	ed::Resume();

	ed::Suspend();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

	if (ImGui::BeginPopup("Create New Node"))
	{
		auto newNodePostion = openPopupPosition;
		CNodeType** defaultTypes = CNodeTypeCollector::GetAllNodeTypes(CNodeType::ENodeType::EDefault);
		CNodeType** customTypes = CNodeTypeCollector::GetAllNodeTypes(CNodeType::ENodeType::ECustom);
		CNodeType** childTypes = CNodeTypeCollector::GetAllNodeTypes(CNodeType::ENodeType::EChild);
		unsigned short noOfDefaultTypes = CNodeTypeCollector::GetNodeTypeCount(CNodeType::ENodeType::EDefault);
		unsigned short noOfCustomTypes = CNodeTypeCollector::GetNodeTypeCount(CNodeType::ENodeType::ECustom);
		unsigned short noOfChildTypes = CNodeTypeCollector::GetNodeTypeCount(CNodeType::ENodeType::EChild);

		std::map< std::string, std::vector<CNodeType*>> cats;
		for (int i = 0; i < noOfDefaultTypes; i++)
			cats[defaultTypes[i]->GetNodeTypeCategory()].push_back(defaultTypes[i]);

		static bool noVariablesCreated = true;
		for (int i = 0; i < noOfCustomTypes; i++)
		{
			cats[customTypes[i]->GetNodeTypeCategory()].push_back(customTypes[i]);
			if (customTypes[i]->GetNodeTypeCategory() == "New Node Type")
				noVariablesCreated = false;
		}

		for (int i = 0; i < noOfChildTypes; i++)
			cats[childTypes[i]->GetNodeTypeCategory()].push_back(childTypes[i]);

		if (noVariablesCreated)
		{
			cats["New Node Type"].push_back(nullptr);
			cats["Delete Node Type"].push_back(nullptr);
		}

		ImGui::PushItemWidth(100.0f);
		ImGui::InputText("##edit", (char*)myMenuSearchField, 127);

		if (mySearchFokus)
			ImGui::SetKeyboardFocusHere(0);

		mySearchFokus = false;
		ImGui::PopItemWidth();

		if (myMenuSearchField[0] != '\0')
		{
			std::vector<CNodeType*> found;

			PopulateNodeList(found, defaultTypes, noOfDefaultTypes);
			PopulateNodeList(found, customTypes, noOfCustomTypes);
			PopulateNodeList(found, childTypes, noOfChildTypes);

			for (int i = 0; i < found.size(); i++)
			{
				CNodeInstance* node = nullptr;
				if (ImGui::MenuItem(found[i]->NodeName().c_str()))
				{
					node = new CNodeInstance(this);
					node->NodeType(found[i]);
					node->CheckIfInputNode();
					node->ConstructUniquePins();
					ed::SetNodePosition(node->UID().AsInt(), newNodePostion);
					node->HasSetEditorPosition(true);

					myCurrentGraph->myNodeInstances.push_back(node);

					if (myPushCommand)
					{
						mySave = true;
						myUndoCommands.push({ ECommandAction::ECreate, node, nullptr, {0,0,0}, node->UID().AsInt() });
					}
				}
			}
		}
		else
		{
			for (auto& category : cats)
			{
				std::string theCatName = category.first;
				if (theCatName.empty())
					theCatName = "General";

				if (ImGui::BeginMenu(theCatName.c_str()))
				{
					CNodeInstance* node = nullptr;

					if (theCatName == "New Node Type")
					{
						for (const auto& instantiableVariable : myInstantiableVariables)
						{
							if (ImGui::MenuItem(instantiableVariable.c_str()))
							{
								myNewVariableType = instantiableVariable;
								myEnteringNodeName = true;
							}
						}
					}
					else if (theCatName == "Delete Node Type")
					{
						for (int i = 0; i < myCustomDataNodes.size(); ++i)
						{
							if (ImGui::MenuItem(myCustomDataNodes[i].c_str()))
							{
								for (auto& graph : myGraphs)
								{
									auto it = graph.myNodeInstances.begin();
									while (it != graph.myNodeInstances.end())
									{
										if ((*it)->NodeType()->NodeName() == "Get: " + myCustomDataNodes[i])
										{
											DeleteNodeType(*(*it));
											it = graph.myNodeInstances.erase(it);
										}
										else if ((*it)->NodeType()->NodeName() == "Set: " + myCustomDataNodes[i])
										{
											DeleteNodeType(*(*it));
											it = graph.myNodeInstances.erase(it);
										}
										else
											++it;
									}
								}
								CNodeTypeCollector::DegisterCustomDataType(myCustomDataNodes[i]);
								std::swap(myCustomDataNodes[i], myCustomDataNodes.back());
								myCustomDataNodes.pop_back();
								CNodeDataManager::Get()->SaveDataTypesToJson();
							}
						}
					}
					else
					{
						for (int i = 0; i < category.second.size(); i++)
						{
							if (category.second[i] == nullptr)
								break;

							CNodeType* type = category.second[i];
							if (ImGui::MenuItem(type->NodeName().c_str()))
							{
								node = new CNodeInstance(this, true);

								//int nodeType = i;
								node->NodeType(type);
								node->CheckIfInputNode();
								node->ConstructUniquePins();
								ed::SetNodePosition(node->UID().AsInt(), newNodePostion);
								node->HasSetEditorPosition(true);

								myCurrentGraph->myNodeInstances.push_back(node);

								if (myPushCommand)
								{
									mySave = true;
									myUndoCommands.push({ ECommandAction::ECreate, node, nullptr, {0,0,0}, node->UID().AsInt() });
								}
							}
						}
					}
					ImGui::EndMenu();
				}
			}
		}
		ImGui::EndPopup();
	}
	else
	{
		mySearchFokus = true;
		memset(&myMenuSearchField[0], 0, sizeof(myMenuSearchField));
	}

	ImGui::PopStyleVar();
	ed::Resume();
}

void CGraphManager::PopulateNodeList(std::vector<CNodeType*>& aNodeListToFill, CNodeType**& aNodeTypeList, const unsigned int& aNumberOfNodes)
{
	for (unsigned int i = 0; i < aNumberOfNodes; i++)
	{
		std::string first = aNodeTypeList[i]->NodeName();
		std::transform(first.begin(), first.end(), first.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });

		std::string second = myMenuSearchField;
		std::transform(second.begin(), second.end(), second.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });

		if (first.find(second) != std::string::npos)
			aNodeListToFill.push_back(aNodeTypeList[i]);
	}
}

void CGraphManager::UndoRedoActions()
{
	if (ed::BeginShortcut())
	{
		if (ed::AcceptCopy())
			mySaveLoadGraphManager->SaveNodesToClipboard(*this);

		if (ed::AcceptPaste())
			mySaveLoadGraphManager->LoadNodesFromClipboard(*this);

		if (ed::AcceptUndo())
		{
			if (!myUndoCommands.empty())
			{
				myPushCommand = false;
				ed::ResetShortCutAction();
				auto& command = myUndoCommands.top();
				myUndoCommands.pop();
				mySave = true;
				mySaveLoadGraphManager->LoadNodesFromClipboard(*this);
				myRedoCommands.push(CreateInverseEditorCommand(command));
			}
		}
		
		if (ed::AcceptRedo())
		{
			if (!myRedoCommands.empty())
			{
				myPushCommand = false;
				ed::ResetShortCutAction();
				auto& command = myRedoCommands.top();
				myRedoCommands.pop();
				mySave = true;
				myUndoCommands.push(CreateInverseEditorCommand(command));
			}
		}
	}
}

void CGraphManager::PostFrame()
{
	if (mySave)
	{
		mySave = false;
		mySaveLoadGraphManager->SaveTreeToFile(*this);
	}

	if (myShowFlow)
		for (int i = 0; i < myCurrentGraph->myLinks.size(); i++)
			ed::Flow(myCurrentGraph->myLinks[i].myID);

	for (auto i : myFlowsToBeShown)
		ed::Flow(i);

	myFlowsToBeShown.clear();
	ed::End();
	ed::SetCurrentEditor(nullptr);
}
#endif // _DEBUG