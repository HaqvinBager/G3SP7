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
#include <iostream>
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
#include "SaveLoadGraphManager.h"
#include "DrawGraphManager.h"
#ifdef _DEBUG
#include <imgui_node_editor.h>
#include "Drawing.h"
#include "Widgets.h"
#include "Interop.h"
#include <imgui_impl_dx11.h>

namespace ed = ax::NodeEditor;
using namespace ax::Drawing;
static ed::EditorContext* g_Context = nullptr;

CGraphManager::~CGraphManager()
{
	delete myHeaderTextureID;
	myHeaderTextureID = nullptr;
	ed::DestroyEditor(g_Context);
}
#endif
using namespace rapidjson;

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
	myRunScripts = false;
#else
	myRunScripts = true;
#endif
	//mySaveLoadGraphManager->GraphManager(this);
	myDrawGraphManager = new CDrawGraphManager();
	myDrawGraphManager->GraphManager(*this);

	if (!CNodeDataManager::Get())
	{
		CNodeDataManager::Create();
		myInstantiableVariables.push_back("Float");
		myInstantiableVariables.push_back("Int");
		myInstantiableVariables.push_back("Bool");
		myInstantiableVariables.push_back("Start");
		myInstantiableVariables.push_back("Vector 3");
	}

	CGraphNodeTimerManager::Create();
	mySaveLoadGraphManager->LoadScripts(*this, aSceneName, mySceneFolder);
	CNodeDataManager::Get()->SetFolderPath(mySceneFolder);

	if (myGraphs.size() > 0)
		myCurrentGraph = &myGraphs[0];
	CNodeTypeCollector::PopulateTypes();
#ifdef _DEBUG
	myHeaderTextureID = nullptr;
	ed::Config config;
	std::string simple = "Imgui/NodeScripts/Simple.json";
	config.SettingsFile = simple.c_str();
	g_Context = ed::CreateEditor(&config);
	myRenderGraph = false;
	myRunScripts = false;
#endif
	LoadDataNodesFromFile();
	myMenuSearchField = new char[127];
	memset(&myMenuSearchField[0], 0, sizeof(myMenuSearchField));
	mySaveLoadGraphManager->LoadTreeFromFile(*this);

	if (myGraphs.size() > 0)
		myCurrentGraph = &myGraphs[0];
}

void CGraphManager::Clear()
{
	if (myGraphs.size() <= 0)
		return;

#ifdef _DEBUG
	mySaveLoadGraphManager->SaveTreeToFile(*this);
#endif // _DEBUG

	CUID::ClearUIDS();

	for (auto& sGraph : myGraphs)
		sGraph.Clear();

	myGraphs.clear();
	myCurrentGraph = nullptr;

	CNodeDataManager::Get()->ClearStoredData();
}

void CGraphManager::ReTriggerUpdatingTrees()
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
					if (nodeInstance->myNodeType->IsStartNode())
						nodeInstance->Enter();
				}
			}
		}
	}
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
#endif
	}
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
		if ((*it)->myUID.AsInt() == anID)
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
			if (pin.myUID.AsInt() == anID)
				return nodeInstance;
	}
	return nullptr;
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
#endif

	ReTriggerUpdatingTrees();

#ifdef _DEBUG
	if (myRenderGraph)
	{
		ed::SetCurrentEditor(g_Context);
		ed::Begin("My Editor", ImVec2(0.0, 0.0f));
	}
#endif
}

bool ArePinTypesCompatible(SPin& aFirst, SPin& aSecond)
{
	if ((aFirst.myVariableType == SPin::EPinType::EFlow && aSecond.myVariableType != SPin::EPinType::EFlow))
		return false;
	if ((aSecond.myVariableType == SPin::EPinType::EFlow && aFirst.myVariableType != SPin::EPinType::EFlow))
		return false;

	return true;
}

void CGraphManager::LoadDataNodesFromFile()
{
	Document document;
	{
		std::string path = mySceneFolder + "CustomDataNodes.json";
		document = CJsonReader::Get()->LoadDocument(path);
		if (CJsonReader::IsValid(document, { "Custom Data" }))
		{
			auto nodeInstances = document["Custom Data"].GetArray();

			for (unsigned int i = 0; i < nodeInstances.Size(); ++i)
			{
				if (nodeInstances[i].HasMember("Type"))
				{
					myNewVariableType = nodeInstances[i]["Type"].GetString();

					RegisterNewDataNode(nodeInstances[i]["Data key"].GetString());
				}
			}
		}
	}
}

void CGraphManager::RegisterNewDataNode(std::string aName)
{
	if (myNewVariableType == "Float")
	{
		float value = 0.0f;
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EFloat));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EFloat, value);
	}
	else if (myNewVariableType == "Int")
	{
		int value = 0;
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EInt));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EInt, value);
	}
	else if (myNewVariableType == "Bool")
	{
		bool value = false;
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EBool));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EBool, value);
	}
	else if (myNewVariableType == "Start")
	{
		bool value = false;
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EStart));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EStart, value);
	}
	else if (myNewVariableType == "Vector 3")
	{
		Vector3 value = { 0.0f,0.0f,0.0f };
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EVector3));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EVector3, value);
	}
	myCustomDataNodes.push_back(aName);
}

void CGraphManager::CurrentGraph(CGraphManager::SGraph* aGraph)
{
	myCurrentGraph = aGraph;
}

void CGraphManager::Graph(SGraph aGraph)
{
	myGraphs.push_back(aGraph);
}

CGraphManager::SGraph CGraphManager::Graph(unsigned int anIndex)
{
	return myGraphs[anIndex];
}

void CGraphManager::Graph(SGraph aGraph, unsigned int anIndex)
{
	myGraphs[anIndex] = aGraph;
}

#ifdef _DEBUG
CGraphManager::EditorCommand CGraphManager::CreateInverseEditorCommand(CGraphManager::EditorCommand &anEditorCommand)
{
	EditorCommand inverseCommand = anEditorCommand;
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
		myCurrentGraph->myNodeInstances.push_back(anEditorCommand.myNodeInstance);
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
		anEditorCommand.myNodeInstance->AddLinkToVia(anEditorCommand.mySecondNodeInstance, static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myInputID.Get()), static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myOutputID.Get()), anEditorCommand.myResourceUID);
		anEditorCommand.mySecondNodeInstance->AddLinkToVia(anEditorCommand.myNodeInstance, static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myOutputID.Get()), static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myInputID.Get()), anEditorCommand.myResourceUID);

		firstPin = anEditorCommand.myNodeInstance->GetPinFromID(static_cast<unsigned int>(anEditorCommand.myEditorLinkInfo.myInputID.Get()));
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
	aNodeInstance.myNodeType->ClearNodeInstanceFromMap(&aNodeInstance);
	std::vector<SNodeInstanceLink> links = (&aNodeInstance)->GetLinks();
	for (auto& link : links)
	{
		CNodeInstance* firstNode = GetNodeFromPinID(static_cast<unsigned int>(link.myFromPinID));
		CNodeInstance* secondNode = GetNodeFromPinID(static_cast<unsigned int>(link.myToPinID));

		firstNode->RemoveLinkToVia(secondNode, static_cast<unsigned int>(link.myFromPinID));
		secondNode->RemoveLinkToVia(firstNode, static_cast<unsigned int>(link.myToPinID));
	}
}

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
				RegisterNewDataNode(buffer);
				CNodeDataManager::Get()->SaveDataTypesToJson();
				ZeroMemory(buffer, 64);
				hasCreatedNewVariable = false;
			}
		}
		ImGui::End();
	}
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

ImTextureID CGraphManager::HeaderTextureID()
{
	if (!myHeaderTextureID)
		myHeaderTextureID = ImGui_LoadTexture("Imgui/Sprites/BlueprintBackground.png");

	return myHeaderTextureID;
}

void CGraphManager::ConstructEditorTreeAndConnectLinks()
{
	for (auto& nodeInstance : myCurrentGraph->myNodeInstances)
	{
		if (!nodeInstance->myHasSetEditorPosition)
		{
			ed::SetNodePosition(nodeInstance->myUID.AsInt(), ImVec2(nodeInstance->myEditorPosition[0], nodeInstance->myEditorPosition[1]));
			nodeInstance->myHasSetEditorPosition = true;
		}

		ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));
		ed::BeginNode(nodeInstance->myUID.AsInt());
		ImGui::PushID(nodeInstance->myUID.AsInt());
		ImGui::BeginVertical("node");

		ImGui::BeginHorizontal("header");
		ImGui::Spring(0);
		ImGui::TextUnformatted(nodeInstance->GetNodeName().c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 28));
		ImGui::Spring(0);

		ImGui::EndHorizontal();
		ax::rect HeaderRect = ImGui_GetItemRect();
		ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);

		bool previusWasOut = false;
		bool isFirstInput = true;
		bool isFirstIteration = true;
		for (auto& pin : nodeInstance->GetPins())
		{
			if (isFirstIteration)
			{
				if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_OUT)
					isFirstInput = false;
				isFirstIteration = false;
			}

			if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
			{
				ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Input);

				ImGui::Text(pin.myText.c_str());
				ImGui::SameLine(0, 0);
				if (pin.myVariableType == SPin::EPinType::EFlow)
					myDrawGraphManager->DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
				else
					myDrawGraphManager->DrawTypeSpecificPin(pin, nodeInstance);

				ed::EndPin();
				previusWasOut = false;

			}
			else
			{
				if (isFirstInput)
					ImGui::SameLine(100, 0);

				ImGui::Indent(150.0f);


				ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Output);

				ImGui::Text(pin.myText.c_str());
				ImGui::SameLine(0, 0);

				myDrawGraphManager->DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
				ed::EndPin();
				previusWasOut = true;
				ImGui::Unindent(150.0f);
				isFirstInput = false;
			}
		}

		ImGui::EndVertical();
		auto ContentRect = ImGui_GetItemRect();
		ed::EndNode();

		if (ImGui::IsItemVisible())
		{
			auto drawList = ed::GetNodeBackgroundDrawList(nodeInstance->myUID.AsInt());

			const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 0.5f;
			auto headerColor = nodeInstance->GetColor();
			const auto uv = ImVec2(
				HeaderRect.w / (float)(4.0f * ImGui_GetTextureWidth(HeaderTextureID())),
				HeaderRect.h / (float)(4.0f * ImGui_GetTextureHeight(HeaderTextureID())));

			drawList->AddImageRounded(HeaderTextureID(),
				to_imvec(HeaderRect.top_left()) - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth),
				to_imvec(HeaderRect.bottom_right()) + ImVec2(8 - halfBorderWidth, 0),
				ImVec2(0.0f, 0.0f), uv,
				headerColor, ed::GetStyle().NodeRounding, 1 | 2);


			auto headerSeparatorRect = ax::rect(HeaderRect.bottom_left(), ContentRect.top_right());
			drawList->AddLine(
				to_imvec(headerSeparatorRect.top_left()) + ImVec2(-(8 - halfBorderWidth), -0.5f),
				to_imvec(headerSeparatorRect.top_right()) + ImVec2((8 - halfBorderWidth), -0.5f),
				ImColor(255, 255, 255, 255), 1.0f);
		}
		ImGui::PopID();
		ed::PopStyleVar();
	}

	for (auto& linkInfo : myCurrentGraph->myLinks)
		ed::Link(linkInfo.myID, linkInfo.myInputID, linkInfo.myOutputID);

	if (!myRunScripts)
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
		ed::EndCreate();

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
						if ((*it)->myUID.AsInt() == nodeId.Get())
						{

							(*it)->myNodeType->ClearNodeInstanceFromMap((*it));
							if (myPushCommand)
							{
								mySave = true;
								myUndoCommands.push({ ECommandAction::EDelete, (*it), nullptr,  {0,0,0}, (*it)->myUID.AsInt() });
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

						node->myNodeType = found[i];
						node->CheckIfInputNode();
						node->ConstructUniquePins();
						ed::SetNodePosition(node->myUID.AsInt(), newNodePostion);
						node->myHasSetEditorPosition = true;

						myCurrentGraph->myNodeInstances.push_back(node);

						if (myPushCommand)
						{
							mySave = true;
							myUndoCommands.push({ ECommandAction::ECreate, node, nullptr, {0,0,0}, node->myUID.AsInt() });
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
											if ((*it)->myNodeType->NodeName() == "Get: " + myCustomDataNodes[i])
											{
												DeleteNodeType(*(*it));
												it = graph.myNodeInstances.erase(it);
											}
											else if ((*it)->myNodeType->NodeName() == "Set: " + myCustomDataNodes[i])
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
									node->myNodeType = type;
									node->CheckIfInputNode();
									node->ConstructUniquePins();
									ed::SetNodePosition(node->myUID.AsInt(), newNodePostion);
									node->myHasSetEditorPosition = true;

									myCurrentGraph->myNodeInstances.push_back(node);

									if (myPushCommand)
									{
										mySave = true;
										myUndoCommands.push({ ECommandAction::ECreate, node, nullptr, {0,0,0}, node->myUID.AsInt() });
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

		myPushCommand = true;

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
#endif