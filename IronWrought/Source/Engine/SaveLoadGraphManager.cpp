#include "stdafx.h"
#include "SaveLoadGraphManager.h"
#include "JsonReader.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <filesystem>
#include "GraphManager.h"
#include "NodeInstance.h"
#include "NodeType.h"
#include "NodeTypes.h"

using namespace rapidjson;

SPin::EPinType LoadPinData(NodeDataPtr& someDataToCopy, rapidjson::Value& someData)
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
		someDataToCopy = new Vector3;
		Vector3 test;
		auto values = someData.GetArray();

		test.x = values[0].GetFloat();
		test.y = values[1].GetFloat();
		test.z = values[2].GetFloat();

		memcpy(someDataToCopy, &test, sizeof(Vector3));
		return SPin::EPinType::EVector3;
	}
	return SPin::EPinType::EUnknown;
}

void CSaveLoadGraphManager::LoadScripts(const std::string& aSceneName, std::string& aSceneFolder)
{
	const std::string sceneJson = ASSETPATH("Assets/Generated/" + aSceneName + "/" + aSceneName + ".json");
	const auto doc = CJsonReader::Get()->LoadDocument(sceneJson);
	if (doc.HasParseError())
		return;
	aSceneFolder = "Imgui/NodeScripts/" + aSceneName + "/";

	if (!std::filesystem::exists(aSceneFolder))
	{
		if (!std::filesystem::create_directory(aSceneFolder.c_str()))
		{
			ENGINE_BOOL_POPUP("Failed to create Directory: %s", aSceneName.c_str());
			return;
		}
	}

	for (auto& scene : doc["Scenes"].GetArray())
	{
		if (scene.HasMember("bluePrints"))
		{
			const auto& bluePrints = scene["bluePrints"].GetArray();
			for (auto& bluePrint : bluePrints)
			{
				if (!bluePrint.HasMember("type"))
					continue;
				if (!bluePrint.HasMember("instances"))
					continue;
				if (!(bluePrint["instances"].GetArray().Size() > 0))
					continue;

				std::string key = bluePrint["type"].GetString();
				CGraphManager::SGraph graph;
				myGraphManager->Graphs().push_back(graph);

				for (const auto& jsonGameObjectID : bluePrint["instances"].GetArray())
				{
					if (!jsonGameObjectID.HasMember("instanceID") && jsonGameObjectID.HasMember("childrenInstanceIDs"))
						continue;

					CGraphManager::BluePrintInstance bpInstance;
					bpInstance.rootID = jsonGameObjectID["instanceID"].GetInt();
					bool firstLoop = true;
					int counter = 1;
					for (const auto& childID : jsonGameObjectID["childrenInstanceIDs"].GetArray())
					{
						bpInstance.childrenIDs.emplace_back(childID.GetInt());
						if (firstLoop)
						{
							firstLoop = false;
							continue;
						}
						myGraphManager->Graphs().back().myChildrenKey = key;
						CNodeTypeCollector::RegisterChildNodeTypes(key, counter++, childID.GetInt());
					}
					myGraphManager->Graphs().back().myBluePrintInstances.emplace_back(bpInstance);
				}

				std::string scriptFolder = aSceneFolder + key + "/";
				myGraphManager->Graphs().back().myFolderPath = scriptFolder;
				if (std::filesystem::exists(scriptFolder))
					continue;

				if (!std::filesystem::create_directory(scriptFolder.c_str()))
				{
					ENGINE_BOOL_POPUP("Failed to create Directory: %s", scriptFolder.c_str());
					continue;
				}
				else
				{
					myGraphManager->CurrentGraph(myGraphManager->Graphs().back());
#ifdef _DEBUG
					SaveTreeToFile();
#endif // _DEBUG
				}
			}
		}
	}
}

void CSaveLoadGraphManager::LoadTreeFromFile()
{
	CUID::myAllUIDs.clear();
	CUID::myGlobalUID = 0;
	for (auto& graph : myGraphManager->Graphs())
	{
		myGraphManager->CurrentGraph(graph);
		Document document;
		{
			std::string path = graph.myFolderPath + "nodeinstances.json";

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
					CNodeInstance* object = new CNodeInstance(myGraphManager, false);
					int nodeTypeID = nodeInstance["NodeType ID"].GetInt();
					int UID = nodeInstance["UID"].GetInt();
					object->myUID.SetUID(UID);
					object->myNodeType = CNodeTypeCollector::GetNodeTypeFromID(nodeTypeID, static_cast<CNodeType::ENodeType>(nodeInstance["NodeType"].GetInt()));

					if (object->myNodeType)
						object->CheckIfInputNode();

					object->myEditorPosition[0] = static_cast<float>(nodeInstance["Position"]["X"].GetInt());
					object->myEditorPosition[1] = static_cast<float>(nodeInstance["Position"]["Y"].GetInt());

					object->ConstructUniquePins();

					for (unsigned int j = 0; j < nodeInstance["Pins"].Size(); j++)
					{
						int index = nodeInstance["Pins"][j]["Index"].GetInt();
						object->myPins[index].myUID.SetUID(nodeInstance["Pins"][j]["UID"].GetInt());
						SPin::EPinType newType = LoadPinData(object->myPins[index].myData, nodeInstance["Pins"][j]["DATA"]);
						if (object->myPins[index].myVariableType == SPin::EPinType::EUnknown)
							object->ChangePinTypes(newType);
					}
					graph.myNodeInstances.push_back(object);
				}
			}
		}
		{
			document = CJsonReader::Get()->LoadDocument(graph.myFolderPath + "links.json");
			if (document.HasMember("Links"))
			{
				auto links = document["Links"].GetArray();
				graph.myNextLinkIdCounter = 0;
				for (unsigned int i = 0; i < links.Size(); i++)
				{
					unsigned int id = document["Links"][i]["ID"].GetInt();
					int inputID = document["Links"][i]["Input"].GetInt();
					int Output = document["Links"][i]["Output"].GetInt();

					CNodeInstance* firstNode = myGraphManager->GetNodeFromPinID(inputID);
					if (!firstNode)
						continue;

					CNodeInstance* secondNode = myGraphManager->GetNodeFromPinID(Output);
					if (!secondNode)
						continue;

					firstNode->AddLinkToVia(secondNode, inputID, Output, id);
					secondNode->AddLinkToVia(firstNode, Output, inputID, id);
#ifdef _DEBUG
					graph.myLinks.push_back({ ed::LinkId(id), ed::PinId(inputID), ed::PinId(Output) });
					if (graph.myNextLinkIdCounter < id + 1)
						graph.myNextLinkIdCounter = id + 1;
#endif
				}
			}
		}
	}

	if (myGraphManager->Graphs().size() > 0)
		myGraphManager->CurrentGraph(myGraphManager->Graphs()[0]);
}

void CSaveLoadGraphManager::SaveTreeToFile()
{
	for (const auto& graph : myGraphManager->Graphs())
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
			for (auto& nodeInstance : graph.myNodeInstances)
				nodeInstance->Serialize(writer1);

			writer1.EndArray();
			writer1.EndObject();

			std::ofstream of(graph.myFolderPath + "/nodeinstances.json");
			of << s.GetString();
		}
		{
			rapidjson::StringBuffer s;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

			writer1.StartObject();
			writer1.Key("Links");
			writer1.StartArray();
			for (auto& link : graph.myLinks)
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

			std::ofstream of(graph.myFolderPath + "/links.json");
			of << s.GetString();
		}
	}
}

void CSaveLoadGraphManager::SaveNodesToClipboard()
{
	int selectedObjectCount = ed::GetSelectedObjectCount();
	ed::NodeId* selectedNodeIDs = new ed::NodeId[selectedObjectCount];
	ed::GetSelectedNodes(selectedNodeIDs, selectedObjectCount);
	std::vector<CNodeInstance*> nodeInstances;
	for (int i = 0; i < selectedObjectCount; ++i)
		nodeInstances.push_back(myGraphManager->GetNodeFromNodeID(static_cast<unsigned int>(selectedNodeIDs[i].Get())));

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
		nodeInstance->Serialize(writer1);

	writer1.EndArray();
	writer1.EndObject();

	std::ofstream of("Imgui/NodeScripts/clipboard.json");
	of << s.GetString();
}

void CSaveLoadGraphManager::LoadNodesFromClipboard()
{
	std::ifstream inputFile("Imgui/NodeScripts/clipboard.json");
	std::stringstream jsonDocumentBuffer;
	std::string inputLine;

	while (std::getline(inputFile, inputLine))
		jsonDocumentBuffer << inputLine << "\n";

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

		CNodeInstance* object = new CNodeInstance(myGraphManager, true);
		int nodeTypeID = nodeInstance["NodeType ID"].GetInt();
		object->myNodeType = CNodeTypeCollector::GetNodeTypeFromID(nodeTypeID, static_cast<CNodeType::ENodeType>(nodeInstance["NodeType"].GetInt()));

		if (object->myNodeType)
			object->CheckIfInputNode();

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
			SPin::EPinType newType = LoadPinData(object->myPins[index].myData, nodeInstance["Pins"][j]["DATA"]);
			if (object->myPins[index].myVariableType == SPin::EPinType::EUnknown)
				object->ChangePinTypes(newType);
		}

		ed::SetNodePosition(object->myUID.AsInt(), position);
		object->myHasSetEditorPosition = true;

		myGraphManager->CurrentGraph().myNodeInstances.push_back(object);
	}
}