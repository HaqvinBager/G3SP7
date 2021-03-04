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
#include <imgui_node_editor.h>
#include "Drawing.h"
#include "Widgets.h"
#include "Interop.h"
#include "NodeTypes.h"
#include <imgui_impl_dx11.h>
#include "Input.h"
#include <filesystem>
#include "Scene.h"
#include "Engine.h"
#include "GameObject.h"
#include "GraphNodeTimerManager.h"
#include "FolderUtility.h"
#include "NodeDataManager.h"

using namespace rapidjson;
namespace ed = ax::NodeEditor;

using namespace ax::Drawing;
static ed::EditorContext* g_Context = nullptr;
CGraphManager::~CGraphManager()
{
	delete myHeaderTextureID;
	myHeaderTextureID = nullptr;
	ed::DestroyEditor(g_Context);
}


void CGraphManager::Load()
{
	if (!CNodeDataManager::Get())
		CNodeDataManager::Create();
	myInstantiableVariables.push_back("Float");
	myInstantiableVariables.push_back("Int");
	myInstantiableVariables.push_back("Bool");
	myInstantiableVariables.push_back("Start");

	CGraphNodeTimerManager::Create();
	for (const auto& blueprintLinksJsonPath : CFolderUtility::GetFileNamesInFolder(ASSETPATH("Assets/Generated"), "BluePrintLinks")) {
		const auto doc = CJsonReader::Get()->LoadDocument(ASSETPATH("Assets/Generated/" + blueprintLinksJsonPath));
		if (doc.HasParseError())
			continue;

		auto jsonObject = doc.GetObjectW();
		if (jsonObject.HasMember("links")) {
			for (const auto& jsonLink : jsonObject["links"].GetArray()) {

				if (!jsonLink.HasMember("type"))
					continue;
				if (!jsonLink.HasMember("instances"))
					continue;
				if (!(jsonLink["instances"].GetArray().Size() > 0))
					continue;

				std::string key = jsonLink["type"].GetString();
				myKeys.push_back(key);

				for (const auto& jsonGameObjectID : jsonLink["instances"].GetArray()) {
					if (!jsonGameObjectID.HasMember("instanceID") && jsonGameObjectID.HasMember("childrenInstanceIDs")) {
						continue;
					}
					BluePrintInstance bpInstance;
					bpInstance.rootID = jsonGameObjectID["instanceID"].GetInt();
					for (const auto& childID : jsonGameObjectID["childrenInstanceIDs"].GetArray()) {
						bpInstance.childrenIDs.emplace_back(childID.GetInt());
					}
					myGameObjectIDsMap[key].emplace_back(bpInstance);
				}

				std::string folder = "Imgui/NodeScripts/" + key;
				//Om denna Blueprint redan finns ska vi bara spara undan den som nyckel
				if (std::filesystem::exists(folder))
					continue;

				if (!std::filesystem::create_directory(folder.c_str()))
				{
					ENGINE_BOOL_POPUP("Failed to create Directory: %s", folder.c_str());
					continue;
				}
				else
				{
					myCurrentKey = myKeys.back();
					myCurrentPath = folder + "/" + myCurrentKey;
					SaveTreeToFile();
				}
			}
		}
	}

	myHeaderTextureID = nullptr;
	ed::Config config;
	std::string simple = "Imgui/NodeScripts/Simple.json";
	config.SettingsFile = simple.c_str();
	g_Context = ed::CreateEditor(&config);
	CNodeTypeCollector::PopulateTypes();
	LoadDataNodesFromFile();
	myMenuSeachField = new char[127];
	memset(&myMenuSeachField[0], 0, sizeof(myMenuSeachField));
	LoadTreeFromFile();


	myShouldRenderGraph = false;
	myScriptShouldRun = false;
}

void CGraphManager::ReTriggerUpdatingTrees()
{
	//Locate start nodes, we support N start nodes, we might want to remove this, as we dont "support" different trees with different starrtnodes to be connected. It might work, might not
	if (myScriptShouldRun)
	{
		for (const auto& key : myKeys)
		{
			const auto& currentGraph = myGraphs[key];
			const auto& gameObjectIDs = myGameObjectIDsMap[key];

			for (unsigned int i = 0; i < gameObjectIDs.size(); ++i)
			{
				myCurrentBluePrintInstance = gameObjectIDs[i];
				for (auto& nodeInstance : currentGraph)
				{
					if (nodeInstance->myNodeType->IsStartNode())
					{
						nodeInstance->Enter();
					}
				}
			}
		}
	}
}

void CGraphManager::SaveTreeToFile()
{
	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

		writer1.StartObject();
		writer1.Key("UID_MAX");

		writer1.StartObject();
		writer1.Key("Num");
		writer1.Int(CUID::myGlobalUID);
		writer1.EndObject();

		writer1.Key("NodeInstances");
		writer1.StartArray();
		for (auto& nodeInstance : myGraphs[myCurrentKey])
		{
			nodeInstance->Serialize(writer1);
		}
		writer1.EndArray();
		writer1.EndObject();



		std::ofstream of(myCurrentPath + "_nodeinstances.json");
		of << s.GetString();
	}
	//Links
	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

		writer1.StartObject();
		writer1.Key("Links");
		writer1.StartArray();
		for (auto& link : myLinks)
		{
			writer1.StartObject();
			writer1.Key("ID");
			writer1.Int(static_cast<int>(link.myID.Get()));
			writer1.Key("Input");
			writer1.Int(static_cast<int>(link.myInputID.Get()));
			writer1.Key("Output");
			writer1.Int(static_cast<int>(link.myOutputID.Get()));
			writer1.EndObject();

		}
		writer1.EndArray();
		writer1.EndObject();


		std::ofstream of(myCurrentPath + "_links.json");
		of << s.GetString();
	}
}

SPin::EPinType LoadPinData(NodeDataPtr& someDataToCopy, rapidjson::Value& someData, Type /*aDataType*/)
{
	if (someData.IsBool())
	{
		someDataToCopy = new bool;
		bool test = someData.GetBool();
		memcpy(someDataToCopy, &test, sizeof(bool));
		return SPin::EPinType::EBool;
	}
	else if (someData.IsDouble())
	{
		someDataToCopy = new float;
		float test = static_cast<float>(someData.GetDouble());
		memcpy(someDataToCopy, &test, sizeof(float));
		return SPin::EPinType::EFloat;
	}
	else if (someData.IsString())
	{
		if (someData.GetStringLength() > 0)
		{
			int length = someData.GetStringLength();
			someDataToCopy = new char[length];
			const char* data = someData.GetString();
			memcpy(someDataToCopy, data, sizeof(char) * length);
			((char*)someDataToCopy)[length] = '\0';
			return SPin::EPinType::EString;
		}

	}
	else if (someData.IsInt())
	{
		someDataToCopy = new int;
		int test = someData.GetInt();
		memcpy(someDataToCopy, &test, sizeof(int));
		return SPin::EPinType::EInt;
	}
	else if (someData.IsArray())
	{
		someDataToCopy = new int;
		int test = someData.GetInt();
		memcpy(someDataToCopy, &test, sizeof(int));
		return SPin::EPinType::EInt;
	}
	return SPin::EPinType::EUnknown;
}

void CGraphManager::LoadTreeFromFile()
{
	if (myKeys.size() > 0)
	{
		for (auto key : myKeys)
		{
			myCurrentKey = key;
			myCurrentPath = "Imgui/NodeScripts/" + key + "/" + key;

			CUID::myAllUIDs.clear();
			CUID::myGlobalUID = 0;
			Document document;
			{
				std::string path = myCurrentPath + "_nodeinstances.json";
				document = CJsonReader::Get()->LoadDocument(path);
				if (document.HasMember("UID_MAX"))
				{
					auto uIDMax = document["UID_MAX"]["Num"].GetInt();
					CUID::myGlobalUID = uIDMax;
				}
				if (document.HasMember("NodeInstances"))
				{
					auto nodeInstances = document["NodeInstances"].GetArray();

					for (unsigned int i = 0; i < nodeInstances.Size(); ++i)
					{
						auto nodeInstance = nodeInstances[i].GetObjectW();
						CNodeInstance* object = new CNodeInstance(this, myCurrentKey, false);
						int nodeType = nodeInstance["NodeType"].GetInt();
						int UID = nodeInstance["UID"].GetInt();
						object->myUID = UID;
						object->myNodeType = CNodeTypeCollector::GetNodeTypeFromID(nodeType);


						object->myEditorPosition[0] = static_cast<float>(nodeInstance["Position"]["X"].GetInt());
						object->myEditorPosition[1] = static_cast<float>(nodeInstance["Position"]["Y"].GetInt());

						object->ConstructUniquePins();

						for (unsigned int j = 0; j < nodeInstance["Pins"].Size(); j++)
						{
							int index = nodeInstance["Pins"][j]["Index"].GetInt();
							object->myPins[index].myUID.SetUID(nodeInstance["Pins"][j]["UID"].GetInt());
							SPin::EPinType newType = LoadPinData(object->myPins[index].myData, nodeInstance["Pins"][j]["DATA"], nodeInstance["Pins"][j]["DATA"].GetType());
							if (object->myPins[index].myVariableType == SPin::EPinType::EUnknown)
							{
								object->ChangePinTypes(newType);
							}
						}
						myGraphs[myCurrentKey].push_back(object);
					}
				}
			}
			{
				document = CJsonReader::Get()->LoadDocument(myCurrentPath + "_links.json");
				if (document.HasMember("Links"))
				{
					auto links = document["Links"].GetArray();
					myNextLinkIdCounter = 0;
					for (unsigned int i = 0; i < links.Size(); i++)
					{
						int id = document["Links"][i]["ID"].GetInt();
						int inputID = document["Links"][i]["Input"].GetInt();
						int Output = document["Links"][i]["Output"].GetInt();

						CNodeInstance* firstNode = GetNodeFromPinID(inputID);
						CNodeInstance* secondNode = GetNodeFromPinID(Output);

						firstNode->AddLinkVia(secondNode, inputID, Output, id);
						secondNode->AddLinkVia(firstNode, Output, inputID, id);

						myLinks.push_back({ ed::LinkId(id), ed::PinId(inputID), ed::PinId(Output) });
						if (myNextLinkIdCounter < id + 1)
						{
							myNextLinkIdCounter = id + 1;
						}
					}
				}
			}
		}

		myCurrentKey = myKeys[0];
		myCurrentPath = "Imgui/NodeScripts/" + myKeys[0] + "/" + myKeys[0];
	}
}

void CGraphManager::SaveNodesToClipboard()
{
	int selectedObjectCount = ed::GetSelectedObjectCount();
	ed::NodeId* selectedNodeIDs = new ed::NodeId[selectedObjectCount];
	ed::GetSelectedNodes(selectedNodeIDs, selectedObjectCount);
	std::vector<CNodeInstance*> nodeInstances;
	for (int i = 0; i < selectedObjectCount; ++i)
		nodeInstances.push_back(GetNodeFromNodeID(static_cast<unsigned int>(selectedNodeIDs[i].Get())));

	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

		writer1.StartObject();
		writer1.Key("UID_MAX");

		writer1.StartObject();
		writer1.Key("Num");
		writer1.Int(CUID::myGlobalUID);
		writer1.EndObject();

		writer1.Key("NodeInstances");
		writer1.StartArray();
		for (auto& nodeInstance : nodeInstances)
		{
			nodeInstance->Serialize(writer1);
		}
		writer1.EndArray();
		writer1.EndObject();



		std::ofstream of("Imgui/NodeScripts/clipboard.json");
		of << s.GetString();
	}
}

void CGraphManager::LoadNodesFromClipboard()
{
	std::ifstream inputFile("Imgui/NodeScripts/clipboard.json");
	std::stringstream jsonDocumentBuffer;
	std::string inputLine;

	while (std::getline(inputFile, inputLine))
	{
		jsonDocumentBuffer << inputLine << "\n";
	}
	rapidjson::Document document;
	document.Parse(jsonDocumentBuffer.str().c_str());

	rapidjson::Value& uidmax = document["UID_MAX"];
	int test = uidmax["Num"].GetInt();
	CUID::myGlobalUID = test;

	rapidjson::Value& results = document["NodeInstances"];

	float firstNodePos[2];

	for (rapidjson::SizeType i = 0; i < results.Size(); i++)
	{
		rapidjson::Value& nodeInstance = results[i];


		CNodeInstance* object = new CNodeInstance(this, myCurrentKey, true);
		int nodeType = nodeInstance["NodeType"].GetInt();
		object->myNodeType = CNodeTypeCollector::GetNodeTypeFromID(nodeType);

		if (i == 0)
		{
			firstNodePos[0] = static_cast<float>(nodeInstance["Position"]["X"].GetInt());
			firstNodePos[1] = static_cast<float>(nodeInstance["Position"]["Y"].GetInt());
		}

		ImVec2 position;
		position.x = nodeInstance["Position"]["X"].GetInt() - firstNodePos[0] + ImGui::GetMousePos().x;
		position.y = nodeInstance["Position"]["Y"].GetInt() - firstNodePos[1] + ImGui::GetMousePos().y;

		object->ConstructUniquePins();

		for (unsigned int j = 0; j < nodeInstance["Pins"].Size(); j++)
		{
			int index = nodeInstance["Pins"][j]["Index"].GetInt();
			object->myPins[index].myUID.SetUID(CUID().AsInt());
			SPin::EPinType newType = LoadPinData(object->myPins[index].myData, nodeInstance["Pins"][j]["DATA"], nodeInstance["Pins"][j]["DATA"].GetType());
			if (object->myPins[index].myVariableType == SPin::EPinType::EUnknown)
			{
				object->ChangePinTypes(newType);
			}
		}

		ed::SetNodePosition(object->myUID.AsInt(), position);
		object->myHasSetEditorPosition = true;

		myGraphs[myCurrentKey].push_back(object);
	}
}

void CGraphManager::ShowFlow(int aLinkID)
{
	if (aLinkID == 0)
		return;
	myFlowsToBeShown.push_back(aLinkID);
}

void CGraphManager::Update()
{
	CGraphNodeTimerManager::Get()->Update();

	PreFrame(CTimer::Dt());
	if (myShouldRenderGraph)
	{
		ConstructEditorTreeAndConnectLinks();
		PostFrame();
		ImGui::End();
	}
}

void CGraphManager::ToggleShouldRenderGraph()
{
	myShouldRenderGraph = !myShouldRenderGraph;
}

bool CGraphManager::ToggleShouldRunScripts()
{
	myScriptShouldRun = !myScriptShouldRun;
	return myScriptShouldRun;
}

std::vector<CGameObject*> CGraphManager::GetCurrentGameObject()
{
	CScene& scene = CEngine::GetInstance()->GetActiveScene();
	std::vector<CGameObject*> gameObjects = {};
	gameObjects.push_back(scene.FindObjectWithID(myCurrentBluePrintInstance.rootID));
	for (const auto& id : myCurrentBluePrintInstance.childrenIDs)
		gameObjects.push_back(scene.FindObjectWithID(id));

	return gameObjects;
}

ImColor GetIconColor(SPin::EPinType type)
{
	switch (type)
	{
	default:
	case SPin::EPinType::EFlow:     return ImColor(255, 255, 255);
	case SPin::EPinType::EBool:     return ImColor(220, 48, 48);
	case SPin::EPinType::EInt:      return ImColor(68, 201, 156);
	case SPin::EPinType::EFloat:    return ImColor(147, 226, 74);
	case SPin::EPinType::EString:   return ImColor(124, 21, 153);
	case SPin::EPinType::EVector3:   return ImColor(124, 21, 153);
	case SPin::EPinType::EUnknown:   return ImColor(255, 0, 0);
	}
};

void DrawPinIcon(const SPin& pin, bool connected, int alpha)
{
	IconType iconType;
	ImColor  color = GetIconColor(pin.myVariableType);
	color.Value.w = alpha / 255.0f;
	switch (pin.myVariableType)
	{
	case SPin::EPinType::EFlow:     iconType = IconType::Flow;   break;
	case SPin::EPinType::EBool:     iconType = IconType::Circle; break;
	case SPin::EPinType::EInt:      iconType = IconType::Circle; break;
	case SPin::EPinType::EFloat:    iconType = IconType::Circle; break;
	case SPin::EPinType::EString:   iconType = IconType::Circle; break;
	case SPin::EPinType::EVector3:   iconType = IconType::Circle; break;
	case SPin::EPinType::EUnknown:  iconType = IconType::Circle; break;
	default:
		return;
	}
	const int s_PinIconSize = 24;
	ax::Widgets::Icon(ImVec2(s_PinIconSize, s_PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
};

CNodeInstance* CGraphManager::GetNodeFromNodeID(unsigned int anID)
{
	if (anID == 0)
		return nullptr;

	auto it = myGraphs[myCurrentKey].begin();
	while (it != myGraphs[myCurrentKey].end())
	{
		if ((*it)->myUID.AsInt() == anID)
		{
			return *it;
		}
		else
		{
			++it;
		}
	}

	return nullptr;
}


CNodeInstance* CGraphManager::GetNodeFromPinID(unsigned int anID)
{
	if (anID == 0)
		return nullptr;

	for (auto& nodeInstance : myGraphs[myCurrentKey])
	{
		std::vector<SPin>& pins = nodeInstance->GetPins();

		for (auto& pin : pins)
		{
			if (pin.myUID.AsInt() == anID)
			{
				return nodeInstance;
			}
		}

	}
	return nullptr;
}


void CGraphManager::DrawTypeSpecificPin(SPin& aPin, CNodeInstance* aNodeInstance)
{
	switch (aPin.myVariableType)
	{
	case SPin::EPinType::EString:
	{
		if (!aPin.myData)
		{
			aPin.myData = new char[128];
			static_cast<char*>(aPin.myData)[0] = '\0';
		}

		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			ImGui::InputText("##edit", (char*)aPin.myData, 127);
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EInt:
	{
		if (!aPin.myData)
		{
			aPin.myData = new int;
			int* c = ((int*)aPin.myData);
			*c = 0;
		}
		int* c = ((int*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			ImGui::InputInt("##edit", c);
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EBool:
	{
		if (!aPin.myData)
		{
			aPin.myData = new bool;
			bool* c = ((bool*)aPin.myData);
			*c = false;
		}
		bool* c = ((bool*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			ImGui::Checkbox("##edit", c);
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EFloat:
	{
		if (!aPin.myData)
		{
			aPin.myData = new float;
			float* c = ((float*)aPin.myData);
			*c = 1.0f;
		}
		float* c = ((float*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(70.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			ImGui::InputFloat("##edit", c);
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EVector3:
	{
		if (!aPin.myData)
		{
			aPin.myData = new float;
			float* c = ((float*)aPin.myData);
			*c = 1.0f;
		}
		float* c = ((float*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(70.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			ImGui::InputFloat("##edit", c);
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EUnknown:
	{
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);

		int selectedIndex = -1;
		if (ImGui::RadioButton("Bool", false))
		{
			selectedIndex = (int)SPin::EPinType::EBool;
		}
		if (ImGui::RadioButton("Int", false))
		{
			selectedIndex = (int)SPin::EPinType::EInt;
		}
		if (ImGui::RadioButton("Float", false))
		{
			selectedIndex = (int)SPin::EPinType::EFloat;
		}
		if (ImGui::RadioButton("Vector3", false))
		{
			selectedIndex = (int)SPin::EPinType::EVector3;
		}
		if (ImGui::RadioButton("String", false))
		{
			selectedIndex = (int)SPin::EPinType::EString;
		}

		if (selectedIndex != -1)
		{
			CNodeInstance* instance = GetNodeFromPinID(aPin.myUID.AsInt());
			instance->ChangePinTypes((SPin::EPinType)selectedIndex);
		}


		ImGui::PopItemWidth();
		ImGui::PopID();
		break;
	}
	default:
		assert(0);
	}

}

void CGraphManager::CreateNewDataNode()
{
	if (myIsEnteringNodeName)
	{
		if (!myHasSetPosition)
		{
			ImGui::SetNextWindowPos({ ImGui::GetIO().MousePos.x,ImGui::GetIO().MousePos.y });
			myHasSetPosition = true;
		}
		ImGui::SetNextWindowSize({ 225, 60 });
		ImGui::Begin(myNewVariableType.c_str());
		static char buffer[64] = "";
		ImGui::InputText("Name", buffer, 64);

		if (Input::GetInstance()->IsKeyPressed(VK_RETURN))
		{
			myHasSetPosition = false;
			myIsEnteringNodeName = false;
			bool hasCreatedNewVariable = false;
			if (strlen(buffer) == 0)
				memcpy(buffer, myNewVariableType.c_str(), 10);

			for (unsigned int i = 0; i < CNodeTypeCollector::GetNodeTypeCount(); ++i)
			{
				if (!hasCreatedNewVariable)
				{
					if (CNodeTypeCollector::GetAllNodeTypes()[i]->NodeName() == buffer)
					{
						break;
					}
					else
					{
						if (myNewVariableType == "Float")
						{
							float nullValue = 1.0f;
							CNodeTypeCollector::RegisterNewDataType(buffer, static_cast<int>(CNodeDataManager::EDataType::EFloat));
							CNodeDataManager::Get()->SetData(buffer, CNodeDataManager::EDataType::EFloat, nullValue);
						}
						else if (myNewVariableType == "Int")
						{
							int nullValue = 1;
							CNodeTypeCollector::RegisterNewDataType(buffer, static_cast<int>(CNodeDataManager::EDataType::EInt));
							CNodeDataManager::Get()->SetData(buffer, CNodeDataManager::EDataType::EInt, nullValue);
						}
						else if (myNewVariableType == "Bool")
						{
							bool nullValue = true;
							CNodeTypeCollector::RegisterNewDataType(buffer, static_cast<int>(CNodeDataManager::EDataType::EBool));
							CNodeDataManager::Get()->SetData(buffer, CNodeDataManager::EDataType::EBool, nullValue);
						}
						else if (myNewVariableType == "Start")
						{
							bool nullValue = false;
							CNodeTypeCollector::RegisterNewDataType(buffer, static_cast<int>(CNodeDataManager::EDataType::EStart));
							CNodeDataManager::Get()->SetData(buffer, CNodeDataManager::EDataType::EStart, nullValue);
						}
						CNodeDataManager::Get()->SaveDataTypesToJson();
						hasCreatedNewVariable = true;
					}
				}
			}
		}
		ImGui::End();
	}
}

void CGraphManager::LoadDataNodesFromFile()
{
	Document document;
	{
		std::string path = "Imgui/NodeScripts/CustomDataNodes.json";
		document = CJsonReader::Get()->LoadDocument(path);
		if (CJsonReader::IsValid(document, { "Custom Data" }))
		{
			auto nodeInstances = document["Custom Data"].GetArray();

			for (unsigned int i = 0; i < nodeInstances.Size(); ++i)
			{
				if (nodeInstances[i].HasMember("Type"))
				{
					myNewVariableType = nodeInstances[i]["Type"].GetString();

					if (myNewVariableType == "Float")
					{
						float value = 1.0f;
						CNodeTypeCollector::RegisterNewDataType(nodeInstances[i]["Data key"].GetString(), static_cast<int>(CNodeDataManager::EDataType::EFloat));
						CNodeDataManager::Get()->SetData(nodeInstances[i]["Data key"].GetString(), CNodeDataManager::EDataType::EFloat, value);
					}
					else if (myNewVariableType == "Int")
					{
						int value = 1;
						CNodeTypeCollector::RegisterNewDataType(nodeInstances[i]["Data key"].GetString(), static_cast<int>(CNodeDataManager::EDataType::EInt));
						CNodeDataManager::Get()->SetData(nodeInstances[i]["Data key"].GetString(), CNodeDataManager::EDataType::EInt, value);
					}
					else if (myNewVariableType == "Bool")
					{
						bool value = true;
						CNodeTypeCollector::RegisterNewDataType(nodeInstances[i]["Data key"].GetString(), static_cast<int>(CNodeDataManager::EDataType::EBool));
						CNodeDataManager::Get()->SetData(nodeInstances[i]["Data key"].GetString(), CNodeDataManager::EDataType::EBool, value);
					}
					else if (myNewVariableType == "Start")
					{
						bool value = false;
						CNodeTypeCollector::RegisterNewDataType(nodeInstances[i]["Data key"].GetString(), static_cast<int>(CNodeDataManager::EDataType::EStart));
						CNodeDataManager::Get()->SetData(nodeInstances[i]["Data key"].GetString(), CNodeDataManager::EDataType::EStart, value);
					}
				}
			}
		}
	}
}

ImTextureID CGraphManager::HeaderTextureID()
{
	if (!myHeaderTextureID)
	{
		myHeaderTextureID = ImGui_LoadTexture("Imgui/Sprites/BlueprintBackground.png");
	}
	return myHeaderTextureID;
}

void CGraphManager::WillBeCyclic(CNodeInstance* aFirst, CNodeInstance* /*aSecond*/, bool& aIsCyclic, CNodeInstance* aBase)
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
			{
				return;
			}
			for (auto& link : links)
			{
				CNodeInstance* connectedNodeToOutPut = GetNodeFromPinID(link->myToPinID);

				if (connectedNodeToOutPut == aBase)
				{
					aIsCyclic |= true;
					return;
				}
				WillBeCyclic(connectedNodeToOutPut, nullptr, aIsCyclic, aBase);
			}
		}
	}
}

#include "Input.h"
void CGraphManager::PreFrame(float aDeltaTime)
{
	static float timer = 0;
	timer += aDeltaTime;
	static std::string currentScript = "Current: " + myCurrentKey;
	if (myShouldRenderGraph)
	{
		auto& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(0, 18));
		ImGui::SetNextWindowSize({ io.DisplaySize.x,  io.DisplaySize.y });
		ImGui::SetNextWindowBgAlpha(0.5f);

		ImGui::Begin(currentScript.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

		ImGui::SameLine();
		if (ImGui::Button("Save"))
			myLikeToSave = true;

		ImGui::SetNextWindowPos(ImVec2(8.5f, 69.5f));
		ImGui::SetNextWindowSize({ 200.f,  io.DisplaySize.y - 69.5f });

		ImGui::Begin("Scripts:", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		static int selected = -1;

		for (int i = 0; i < myKeys.size(); i++)
		{
			char buf[512];
			sprintf_s(buf, "%s", myKeys[i].c_str());

			if (ImGui::Selectable(buf, selected == i, ImGuiSelectableFlags_AllowDoubleClick))
			{
				selected = i;

				if (ImGui::IsMouseClicked(0))
				{
					myCurrentKey = myKeys[i];
					myCurrentPath = "Imgui/NodeScripts/" + myKeys[i] + "/" + myKeys[i];
					currentScript = "Current: " + myCurrentKey;
				}
			}
		}
		ImGui::End();
	}

	CreateNewDataNode();

	if (myScriptShouldRun)
		ReTriggerUpdatingTrees();

	for (auto& nodeInstance : myGraphs[myCurrentKey])
	{
		nodeInstance->DebugUpdate();
		nodeInstance->VisualUpdate(aDeltaTime);
	}

	static float outRate = 60;

	if (timer > (1.0f / outRate))
	{
		ReTriggerUpdatingTrees();
		timer = 0;
	}

	if (myShouldRenderGraph)
	{
		ed::SetCurrentEditor(g_Context);
		ed::Begin("My Editor", ImVec2(0.0, 0.0f));
	}
}

bool ArePinTypesCompatible(SPin& aFirst, SPin& aSecond)
{
	if ((aFirst.myVariableType == SPin::EPinType::EFlow && aSecond.myVariableType != SPin::EPinType::EFlow))
	{
		return false;
	}
	if ((aSecond.myVariableType == SPin::EPinType::EFlow && aFirst.myVariableType != SPin::EPinType::EFlow))
	{
		return false;
	}
	return true;
}


struct SDistBestResult
{
	size_t myScore;
	CNodeType* ourInstance;
};

struct less_than_key
{
	inline bool operator() (const SDistBestResult& struct1, const SDistBestResult& struct2)
	{
		return (struct1.myScore < struct2.myScore);
	}
};

template<typename T>
size_t uiLevenshteinDistance(const T& source, const T& target)
{
	if (source.size() > target.size()) {
		return uiLevenshteinDistance(target, source);
	}

	using TSizeType = typename T::size_type;
	const TSizeType min_size = source.size(), max_size = target.size();
	std::vector<TSizeType> lev_dist(min_size + 1);

	for (TSizeType i = 0; i <= min_size; ++i) {
		lev_dist[i] = i;
	}

	for (TSizeType j = 1; j <= max_size; ++j) {
		TSizeType previous_diagonal = lev_dist[0], previous_diagonal_save;
		++lev_dist[0];

		for (TSizeType i = 1; i <= min_size; ++i) {
			previous_diagonal_save = lev_dist[i];
			if (source[i - 1] == target[j - 1]) {
				lev_dist[i] = previous_diagonal;
			}
			else {
				lev_dist[i] = min(min(lev_dist[i - 1], lev_dist[i]), previous_diagonal) + 1;
			}
			previous_diagonal = previous_diagonal_save;
		}
	}

	return lev_dist[min_size];
}

void CGraphManager::ConstructEditorTreeAndConnectLinks()
{
	if (myKeys.size() > 0)
	{
		for (auto& nodeInstance : myGraphs[myCurrentKey])
		{


			if (!nodeInstance->myHasSetEditorPosition)
			{
				ed::SetNodePosition(nodeInstance->myUID.AsInt(), ImVec2(nodeInstance->myEditorPosition[0], nodeInstance->myEditorPosition[1]));
				nodeInstance->myHasSetEditorPosition = true;
			}

			// Start drawing nodes.
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
					{
						isFirstInput = false;
					}
					isFirstIteration = false;
				}

				if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
				{
					ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Input);

					ImGui::Text(pin.myText.c_str());
					ImGui::SameLine(0, 0);
					if (pin.myVariableType == SPin::EPinType::EFlow)
					{
						DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
					}
					else
					{
						DrawTypeSpecificPin(pin, nodeInstance);

					}


					ed::EndPin();
					previusWasOut = false;

				}
				else
				{
					if (isFirstInput)
					{
						ImGui::SameLine(100, 0);
					}

					ImGui::Indent(150.0f);


					ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Output);

					ImGui::Text(pin.myText.c_str());
					ImGui::SameLine(0, 0);

					DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
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


		for (auto& linkInfo : myLinks)
			ed::Link(linkInfo.myID, linkInfo.myInputID, linkInfo.myOutputID);

		// Handle creation action, returns true if editor want to create new object (node or link)
		if (ed::BeginCreate())
		{
			ed::PinId inputPinId, outputPinId;
			if (ed::QueryNewLink(&inputPinId, &outputPinId))
			{
				if (inputPinId && outputPinId)
				{
					if (ed::AcceptNewItem())
					{
						CNodeInstance* firstNode = GetNodeFromPinID(static_cast<unsigned int>(inputPinId.Get()));
						CNodeInstance* secondNode = GetNodeFromPinID(static_cast<unsigned int>(outputPinId.Get()));
						assert(firstNode);
						assert(secondNode);

						if (firstNode == secondNode)
						{
							// User trying connect input and output on the same node :/, who does this!?! Indeed Ralmark, indeed....
							// SetBlueScreenOnUserComputer(true)
						}
						else
						{
							{
								SPin* firstPin = firstNode->GetPinFromID(static_cast<unsigned int>(inputPinId.Get()));
								SPin* secondPin = secondNode->GetPinFromID(static_cast<unsigned int>(outputPinId.Get()));

								bool canAddlink = true;
								if (firstPin && secondPin)
								{
									if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN && secondPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
									{
										canAddlink = false;
									}
								}

								if (!ArePinTypesCompatible(*firstPin, *secondPin))
								{
									canAddlink = false;
								}

								if (!firstNode->CanAddLink(static_cast<unsigned int>(inputPinId.Get())))
								{
									canAddlink = false;
								}
								if (!secondNode->CanAddLink(static_cast<unsigned int>(outputPinId.Get())))
								{
									canAddlink = false;
								}

								if (firstNode->HasLinkBetween(static_cast<unsigned int>(inputPinId.Get()), static_cast<unsigned int>(outputPinId.Get())))
								{
									canAddlink = false;
								}


								if (canAddlink)
								{
									if (secondPin->myVariableType == SPin::EPinType::EUnknown)
									{
										secondNode->ChangePinTypes(firstPin->myVariableType);
									}
									int linkId = myNextLinkIdCounter++;
									firstNode->AddLinkVia(secondNode, static_cast<unsigned int>(inputPinId.Get()), static_cast<unsigned int>(outputPinId.Get()), linkId);
									secondNode->AddLinkVia(firstNode, static_cast<unsigned int>(outputPinId.Get()), static_cast<unsigned int>(inputPinId.Get()), linkId);

									bool aIsCyclic = false;
									WillBeCyclic(firstNode, secondNode, aIsCyclic, firstNode);
									if (aIsCyclic || !canAddlink)
									{
										firstNode->RemoveLinkToVia(secondNode, static_cast<unsigned int>(inputPinId.Get()));
										secondNode->RemoveLinkToVia(firstNode, static_cast<unsigned int>(outputPinId.Get()));
									}
									else
									{
										// Depending on if you drew the new link from the output to the input we need to create the link as the flow FROM->TO to visualize the correct flow
										if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
										{
											myLinks.push_back({ ed::LinkId(linkId), outputPinId, inputPinId });
										}
										else
										{
											myLinks.push_back({ ed::LinkId(linkId), inputPinId, outputPinId });
										}

										std::cout << "push add link command!" << std::endl;
										myUndoCommands.push({ ECommandAction::EAddLink, firstNode, secondNode, myLinks.back(), 0 });

										myLikeToSave = true;
										//ReTriggerTree();
									}
								}
							}
						}
					}
				}
			}
		}
		ed::EndCreate();

		// Handle deletion action
		if (ed::BeginDelete())
		{
			// There may be many links marked for deletion, let's loop over them.
			ed::LinkId deletedLinkId;
			while (ed::QueryDeletedLink(&deletedLinkId))
			{
				// If you agree that link can be deleted, accept deletion.
				if (ed::AcceptDeletedItem())
				{
					// Then remove link from your data.
					for (auto& link : myLinks)
					{
						if (link.myID == deletedLinkId)
						{
							CNodeInstance* firstNode = GetNodeFromPinID(static_cast<unsigned int>(link.myInputID.Get()));
							CNodeInstance* secondNode = GetNodeFromPinID(static_cast<unsigned int>(link.myOutputID.Get()));
							assert(firstNode);
							assert(secondNode);

							firstNode->RemoveLinkToVia(secondNode, static_cast<unsigned int>(link.myInputID.Get()));
							secondNode->RemoveLinkToVia(firstNode, static_cast<unsigned int>(link.myOutputID.Get()));

							if (myShouldPushCommand)
							{
								std::cout << "push remove link action!" << std::endl;
								myUndoCommands.push({ ECommandAction::ERemoveLink, firstNode, secondNode, link, 0 });
							}

							myLinks.erase(&link);
							myLikeToSave = true;

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

					auto it = myGraphs[myCurrentKey].begin();
					while (it != myGraphs[myCurrentKey].end())
					{
						if ((*it)->myUID.AsInt() == nodeId.Get())
						{

							(*it)->myNodeType->ClearNodeInstanceFromMap((*it));
							if (myShouldPushCommand)
							{
								std::cout << "Push delete command!" << std::endl;
								myLikeToSave = true;
								myUndoCommands.push({ ECommandAction::EDelete, (*it), nullptr,  {0,0,0}, (*it)->myUID.AsInt() });
							}

							it = myGraphs[myCurrentKey].erase(it);
						}
						else
						{
							++it;
						}
					}

				}
			}
		}
		ed::EndDelete();

		auto openPopupPosition = ImGui::GetMousePos();
		ed::Suspend();

		if (ed::ShowBackgroundContextMenu())
		{
			ImGui::OpenPopup("Create New Node");
		}
		ed::Resume();

		ed::Suspend();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (ImGui::BeginPopup("Create New Node"))
		{

			auto newNodePostion = openPopupPosition;
			CNodeType** types = CNodeTypeCollector::GetAllNodeTypes();
			unsigned short noOfTypes = CNodeTypeCollector::GetNodeTypeCount();

			std::map< std::string, std::vector<CNodeType*>> cats;
			static bool noVariablesCreated = true;
			for (int i = 0; i < noOfTypes; i++)
			{
				cats[types[i]->GetNodeTypeCategory()].push_back(types[i]);
				if (types[i]->GetNodeTypeCategory() == "New Node Type")
					noVariablesCreated = false;
			}

			if (noVariablesCreated)
			{
				cats["New Node Type"].push_back(nullptr);
			}

			ImGui::PushItemWidth(100.0f);
			ImGui::InputText("##edit", (char*)myMenuSeachField, 127);
			if (mySetSearchFokus)
			{
				ImGui::SetKeyboardFocusHere(0);
			}
			mySetSearchFokus = false;
			ImGui::PopItemWidth();

			if (myMenuSeachField[0] != '\0')
			{

				std::vector<SDistBestResult> distanceResults;
				for (int i = 0; i < noOfTypes; i++)
				{
					distanceResults.push_back(SDistBestResult());
					SDistBestResult& dist = distanceResults.back();
					dist.ourInstance = types[i];
					dist.myScore = uiLevenshteinDistance<std::string>(types[i]->NodeName(), myMenuSeachField);
				}

				std::sort(distanceResults.begin(), distanceResults.end(), less_than_key());

				int firstCost = static_cast<int>(distanceResults[0].myScore);
				for (int i = 0; i < distanceResults.size(); i++)
				{
					CNodeInstance* node = nullptr;
					if (ImGui::MenuItem(distanceResults[i].ourInstance->NodeName().c_str()))
					{
						node = new CNodeInstance(this, myCurrentKey);

						node->myNodeType = distanceResults[i].ourInstance;
						node->ConstructUniquePins();
						ed::SetNodePosition(node->myUID.AsInt(), newNodePostion);
						node->myHasSetEditorPosition = true;

						myGraphs[myCurrentKey].push_back(node);

						if (myShouldPushCommand)
						{
							std::cout << "Push create command!" << std::endl;
							myLikeToSave = true;
							myUndoCommands.push({ ECommandAction::ECreate, node, nullptr, {0,0,0}, node->myUID.AsInt() });
						}
					}
					int distance = static_cast<int>(distanceResults[i].myScore) - firstCost;
					if (distance > 3)
					{
						break;
					}
				}

			}
			else
			{
				for (auto& category : cats)
				{
					std::string theCatName = category.first;
					if (theCatName.empty())
					{
						theCatName = "General";
					}

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
									myIsEnteringNodeName = true;
								}
							}
						}
						for (int i = 0; i < category.second.size(); i++)
						{
							if (category.second[i] == nullptr)
								break;

							CNodeType* type = category.second[i];
							if (ImGui::MenuItem(type->NodeName().c_str()))
							{
								node = new CNodeInstance(this, myCurrentKey);

								//int nodeType = i;
								node->myNodeType = type;
								node->ConstructUniquePins();
								ed::SetNodePosition(node->myUID.AsInt(), newNodePostion);
								node->myHasSetEditorPosition = true;

								myGraphs[myCurrentKey].push_back(node);

								if (myShouldPushCommand)
								{
									std::cout << "Push create command!" << std::endl;
									myLikeToSave = true;
									myUndoCommands.push({ ECommandAction::ECreate, node, nullptr, {0,0,0}, node->myUID.AsInt() });
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
			mySetSearchFokus = true;
			memset(&myMenuSeachField[0], 0, sizeof(myMenuSeachField));
		}

		ImGui::PopStyleVar();
		ed::Resume();

		myShouldPushCommand = true;

		if (ed::BeginShortcut())
		{
			if (ed::AcceptCopy())
			{
				SaveNodesToClipboard();
			}

			if (ed::AcceptPaste())
			{
				LoadNodesFromClipboard();
			}

			if (ed::AcceptUndo())
			{
				if (!myUndoCommands.empty())
				{
					myShouldPushCommand = false;
					ed::ResetShortCutAction();
					auto& command = myUndoCommands.top();
					EditorCommand inverseCommand = command;
					SPin* firstPin;
					SPin* secondPin;

					switch (command.myAction)
					{
					case CGraphManager::ECommandAction::ECreate:
						inverseCommand.myAction = ECommandAction::EDelete;
						ed::DeleteNode(command.myResourceUID);
						break;
					case CGraphManager::ECommandAction::EDelete:
						inverseCommand.myAction = ECommandAction::ECreate;
						myGraphs[myCurrentKey].push_back(command.myNodeInstance);
						break;
					case CGraphManager::ECommandAction::EAddLink:
						inverseCommand.myAction = ECommandAction::ERemoveLink;
						ed::DeleteLink(command.myEditorLinkInfo.myID);
						break;
					case CGraphManager::ECommandAction::ERemoveLink:
						inverseCommand.myAction = ECommandAction::EAddLink;
						command.myNodeInstance->AddLinkVia(command.mySecondNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()), command.myResourceUID);
						command.mySecondNodeInstance->AddLinkVia(command.myNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()), command.myResourceUID);

						firstPin = command.myNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()));
						secondPin = command.mySecondNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()));

						if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
							myLinks.push_back({ command.myEditorLinkInfo.myID, command.myEditorLinkInfo.myInputID, command.myEditorLinkInfo.myOutputID });
						else
							myLinks.push_back({ command.myEditorLinkInfo.myID, command.myEditorLinkInfo.myOutputID, command.myEditorLinkInfo.myInputID });
						//ReTriggerTree();
						break;
					default:
						break;
					}
					std::cout << "undo!" << std::endl;
					myUndoCommands.pop();
					std::cout << "Push redo command!" << std::endl;
					myLikeToSave = true;
					myRedoCommands.push(inverseCommand);
				}
			}

			if (ed::AcceptRedo())
			{
				if (!myRedoCommands.empty())
				{
					myShouldPushCommand = false;
					ed::ResetShortCutAction();
					auto& command = myRedoCommands.top();
					EditorCommand inverseCommand = command;
					SPin* firstPin;
					SPin* secondPin;

					switch (command.myAction)
					{
					case CGraphManager::ECommandAction::ECreate:
						inverseCommand.myAction = ECommandAction::EDelete;
						ed::DeleteNode(command.myResourceUID);
						break;
					case CGraphManager::ECommandAction::EDelete:
						inverseCommand.myAction = ECommandAction::ECreate;
						myGraphs[myCurrentKey].push_back(command.myNodeInstance);
						break;
					case CGraphManager::ECommandAction::EAddLink:
						inverseCommand.myAction = ECommandAction::ERemoveLink;
						ed::DeleteLink(command.myEditorLinkInfo.myID);
						break;
					case CGraphManager::ECommandAction::ERemoveLink:
						inverseCommand.myAction = ECommandAction::EAddLink;
						command.myNodeInstance->AddLinkVia(command.mySecondNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()), command.myResourceUID);
						command.mySecondNodeInstance->AddLinkVia(command.myNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()), command.myResourceUID);

						firstPin = command.myNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()));
						secondPin = command.mySecondNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()));

						if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
							myLinks.push_back({ command.myEditorLinkInfo.myID, command.myEditorLinkInfo.myInputID, command.myEditorLinkInfo.myOutputID });
						else
							myLinks.push_back({ command.myEditorLinkInfo.myID, command.myEditorLinkInfo.myOutputID, command.myEditorLinkInfo.myInputID });
						//ReTriggerTree();
						break;
					default:
						break;
					}
					std::cout << "redo!" << std::endl;
					myRedoCommands.pop();
					std::cout << "Push undo command!" << std::endl;
					myLikeToSave = true;
					myUndoCommands.push(inverseCommand);
				}
			}
		}
	}
}

void CGraphManager::PostFrame()
{
	if (myLikeToSave)
	{
		myLikeToSave = false;
		SaveTreeToFile();
	}
	if (myLikeToShowFlow)
	{
		for (int i = 0; i < myLinks.size(); i++)
		{
			ed::Flow(myLinks[i].myID);
		}
	}
	for (auto i : myFlowsToBeShown)
	{
		ed::Flow(i);
	}
	myFlowsToBeShown.clear();
	ed::End();
	ed::SetCurrentEditor(nullptr);
}