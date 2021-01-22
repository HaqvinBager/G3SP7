#include "stdafx.h"
#include "CGraphManager.h"
#include "..\rapidjson/document.h"
#include "..\rapidjson/writer.h"
#include "..\rapidjson/stringbuffer.h"
#include "..\rapidjson/document.h"
#include "..\rapidjson/filewritestream.h"
#include "..\rapidjson/filereadstream.h"
#include "..\rapidjson/prettywriter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "CNodeInstance.h"
#include "CNodeType.h"
#include "Node Editor/imgui_node_editor.h"
#include "Drawing.h"
#include "Widgets.h"
#include "Interop.h"
#include "NodeTypes.h"
#include <imgui_impl_dx11.h>
#include "Input.h"

using namespace rapidjson;
namespace ed = ax::NodeEditor;

using namespace ax::Drawing;
static ed::EditorContext* g_Context = nullptr;
CGraphManager::~CGraphManager()
{
	ed::DestroyEditor(g_Context);
}
void CGraphManager::Load()
{
	ed::Config config;
	config.SettingsFile = "Simple.json";
	g_Context = ed::CreateEditor(&config);
	CNodeTypeCollector::PopulateTypes();
	myMenuSeachField = new char[127];
	memset(&myMenuSeachField[0], 0, sizeof(myMenuSeachField));

}

void CGraphManager::ReTriggerUpdateringTrees()
{
	//Locate start nodes, we support N start nodes, we might want to remove this, as we dont "support" different trees with different starrtnodes to be connected. It might work, might not
	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		if (nodeInstance->myNodeType->IsStartNode() && nodeInstance->myShouldTriggerAgain)
		{
			nodeInstance->Enter();
		}
	}
}

void CGraphManager::ReTriggerTree()
{
	//Locate start nodes, we support N start nodes, we might want to remove this, as we dont "support" different trees with different starrtnodes to be connected. It might work, might not
	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		if (nodeInstance->myNodeType->IsStartNode() && !nodeInstance->myShouldTriggerAgain)
		{
			nodeInstance->Enter();
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
		writer1.Int(UID::myGlobalUID);
		writer1.EndObject();

		writer1.Key("NodeInstances");
		writer1.StartArray();
		for (auto& nodeInstance : myNodeInstancesInGraph)
		{
			nodeInstance->Serialize(writer1);
		}
		writer1.EndArray();
		writer1.EndObject();



		std::ofstream of("nodeinstances.json");
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
			writer1.Int(static_cast<int>(link.Id.Get()));
			writer1.Key("Input");
			writer1.Int(static_cast<int>(link.InputId.Get()));
			writer1.Key("Output");
			writer1.Int(static_cast<int>(link.OutputId.Get()));
			writer1.EndObject();
			
		}
		writer1.EndArray();
		writer1.EndObject();


		std::ofstream of("links.json");
		of << s.GetString();
	}

}

SPin::PinType LoadPinData(NodeDataPtr& dataToKopy, rapidjson::Value& someData, Type /*aDataType*/)
{
	if (someData.IsBool())
	{
		dataToKopy = new bool;
		bool test = someData.GetBool();
		memcpy(dataToKopy, &test, sizeof(bool));
		return SPin::PinType::Bool;
	}
	else if (someData.IsDouble())
	{
		dataToKopy = new float;
		float test = static_cast<float>(someData.GetDouble());
		memcpy(dataToKopy, &test, sizeof(float));
		return SPin::PinType::Float;
	}
	else if (someData.IsString())
	{
		if (someData.GetStringLength() > 0)
		{
			int length = someData.GetStringLength();
			dataToKopy = new char[length];
			const char* data = someData.GetString();
			memcpy(dataToKopy, data, sizeof(char) * length);
			((char*)dataToKopy)[length] = '\0';
			return SPin::PinType::String;
		}

	}
	else if (someData.IsInt())
	{
		dataToKopy = new int;
		int test = someData.GetInt();
		memcpy(dataToKopy, &test, sizeof(int));
		return SPin::PinType::Int;
	}
	return SPin::PinType::Unknown;
}

void CGraphManager::LoadTreeFromFile()
{
	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		delete nodeInstance;
	}

	myLinks.clear();
	myNodeInstancesInGraph.clear();
	UID::myAllUIDs.clear();
	UID::myGlobalUID = 0;
	
	{
		std::ifstream inputFile("nodeinstances.json");
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
		UID::myGlobalUID = test;

		rapidjson::Value& results = document["NodeInstances"];

		for (rapidjson::SizeType i = 0; i < results.Size(); i++)
		{
			rapidjson::Value& nodeInstance = results[i];
			CNodeInstance* object = new CNodeInstance(false);
			int nodeType = nodeInstance["NodeType"].GetInt();
			int UID = nodeInstance["UID"].GetInt();
			object->myUID = UID;
			object->myNodeType = CNodeTypeCollector::GetNodeTypeFromID(nodeType);


			object->myEditorPos[0] = static_cast<float>(nodeInstance["Position"]["X"].GetInt());
			object->myEditorPos[1] = static_cast<float>(nodeInstance["Position"]["Y"].GetInt());
			
			object->ConstructUniquePins();

			for (unsigned int j = 0; j < nodeInstance["Pins"].Size(); j++)
			{
				int index = nodeInstance["Pins"][j]["Index"].GetInt();
				object->myPins[index].myUID.SetUID(nodeInstance["Pins"][j]["UID"].GetInt());
				SPin::PinType newType = LoadPinData(object->myPins[index].myData, nodeInstance["Pins"][j]["DATA"], nodeInstance["Pins"][j]["DATA"].GetType());
				if (object->myPins[index].myVariableType == SPin::PinType::Unknown)
				{
					object->ChangePinTypes(newType);
				}
			}

			myNodeInstancesInGraph.push_back(object);
		}
	}
	{
		std::ifstream inputFile("links.json");
		std::stringstream jsonDocumentBuffer;
		std::string inputLine;

		while (std::getline(inputFile, inputLine))
		{
			jsonDocumentBuffer << inputLine << "\n";
		}
		rapidjson::Document document;
		document.Parse(jsonDocumentBuffer.str().c_str());

		//rapidjson::Value& results = document["Links"];

		myNextLinkIdCounter = 0;
		for (rapidjson::SizeType i = 0; i < document["Links"].Size(); i++)
		{
			int id = document["Links"][i]["ID"].GetInt();
			int inputID = document["Links"][i]["Input"].GetInt();
			int Output = document["Links"][i]["Output"].GetInt();

			CNodeInstance* firstNode = GetNodeFromPinID(inputID);
			CNodeInstance* secondNode = GetNodeFromPinID(Output);

			firstNode->AddLinkToVia(secondNode, inputID, Output, id);
			secondNode->AddLinkToVia(firstNode, Output, inputID, id);

			myLinks.push_back({ ed::LinkId(id), ed::PinId(inputID), ed::PinId(Output) });
			if (myNextLinkIdCounter < id + 1)
			{
				myNextLinkIdCounter = id + 1;
			}
		}
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
		writer1.Int(UID::myGlobalUID);
		writer1.EndObject();

		writer1.Key("NodeInstances");
		writer1.StartArray();
		for (auto& nodeInstance : nodeInstances)
		{
			nodeInstance->Serialize(writer1);
		}
		writer1.EndArray();
		writer1.EndObject();



		std::ofstream of("clipboard.json");
		of << s.GetString();
	}
	//Links
	//{
	//	rapidjson::StringBuffer s;
	//	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

	//	writer1.StartObject();
	//	writer1.Key("Links");
	//	writer1.StartArray();
	//	for (auto& link : myLinks)
	//	{
	//		writer1.StartObject();
	//		writer1.Key("ID");
	//		writer1.Int(link.Id.Get());
	//		writer1.Key("Input");
	//		writer1.Int(link.InputId.Get());
	//		writer1.Key("Output");
	//		writer1.Int(link.OutputId.Get());
	//		writer1.EndObject();

	//	}
	//	writer1.EndArray();
	//	writer1.EndObject();


	//	std::ofstream of("links.json");
	//	of << s.GetString();
	//}
}

void CGraphManager::LoadNodesFromClipboard()
{
	std::ifstream inputFile("clipboard.json");
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
	UID::myGlobalUID = test;

	rapidjson::Value& results = document["NodeInstances"];

	float firstNodePos[2];

	for (rapidjson::SizeType i = 0; i < results.Size(); i++)
	{
		rapidjson::Value& nodeInstance = results[i];
			

		CNodeInstance* object = new CNodeInstance(true);
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
			object->myPins[index].myUID.SetUID(UID().AsInt());
			SPin::PinType newType = LoadPinData(object->myPins[index].myData, nodeInstance["Pins"][j]["DATA"], nodeInstance["Pins"][j]["DATA"].GetType());
			if (object->myPins[index].myVariableType == SPin::PinType::Unknown)
			{
				object->ChangePinTypes(newType);
			}
		}

		ed::SetNodePosition(object->myUID.AsInt(), position);
		object->myHasSetEditorPos = true;
			
		myNodeInstancesInGraph.push_back(object);
	}
}

std::vector<int> myFlowsToBeShown;
void CGraphManager::ShowFlow(int aLinkID)
{
	myFlowsToBeShown.push_back(aLinkID);
}

ImColor GetIconColor(SPin::PinType type)
{
	switch (type)
	{
	default:
		case SPin::PinType::Flow:     return ImColor(255, 255, 255);
		case SPin::PinType::Bool:     return ImColor(220, 48, 48);
		case SPin::PinType::Int:      return ImColor(68, 201, 156);
		case SPin::PinType::Float:    return ImColor(147, 226, 74);
		case SPin::PinType::String:   return ImColor(124, 21, 153);
		case SPin::PinType::Unknown:   return ImColor(255, 0, 0);
	}
};

void DrawPinIcon(const SPin& pin, bool connected, int alpha)
{
	IconType iconType;
	ImColor  color = GetIconColor(pin.myVariableType);
	color.Value.w = alpha / 255.0f;
	switch (pin.myVariableType)
	{
	case SPin::PinType::Flow:     iconType = IconType::Flow;   break;
	case SPin::PinType::Bool:     iconType = IconType::Circle; break;
	case SPin::PinType::Int:      iconType = IconType::Circle; break;
	case SPin::PinType::Float:    iconType = IconType::Circle; break;
	case SPin::PinType::String:   iconType = IconType::Circle; break;
	case SPin::PinType::Unknown:  iconType = IconType::Circle; break;
	default:
		return;
	}
	const int s_PinIconSize = 24;
	ax::Widgets::Icon(ImVec2(s_PinIconSize, s_PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
};

CNodeInstance* CGraphManager::GetNodeFromNodeID(unsigned int aID)
{
	auto it = myNodeInstancesInGraph.begin();
	while (it != myNodeInstancesInGraph.end())
	{
		if ((*it)->myUID.AsInt() == aID)
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


CNodeInstance* CGraphManager::GetNodeFromPinID(unsigned int aID)
{
	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		std::vector<SPin>& pins = nodeInstance->GetPins();

		for (auto& pin : pins)
		{
			if (pin.myUID.AsInt() == aID)
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
		case SPin::PinType::String:
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
				ImGui::InputText("##edit",(char*)aPin.myData, 127);
			}
			ImGui::PopItemWidth();

			ImGui::PopID();
			break;
		}
		case SPin::PinType::Int:
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
		case SPin::PinType::Bool:
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
		case SPin::PinType::Float:
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
		case SPin::PinType::Unknown:
		{
			ImGui::PushID(aPin.myUID.AsInt());
			ImGui::PushItemWidth(100.0f);

			int selectedIndex = -1;
			if (ImGui::RadioButton("Bool", false))
			{
				selectedIndex = (int)SPin::PinType::Bool;
			}
			if (ImGui::RadioButton("Int", false))
			{
				selectedIndex = (int)SPin::PinType::Int;
			}
			if (ImGui::RadioButton("Float", false))
			{
				selectedIndex = (int)SPin::PinType::Float;
			}
			if (ImGui::RadioButton("String", false))
			{
				selectedIndex = (int)SPin::PinType::String;
			}

			if (selectedIndex != -1)
			{
				CNodeInstance* instance = GetNodeFromPinID(aPin.myUID.AsInt());
				instance->ChangePinTypes((SPin::PinType)selectedIndex);
			}

			
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;
		}
		default:
			assert(0);
	}

}

void CGraphManager::WillBeCyclic(CNodeInstance* aFirst, CNodeInstance* /*aSecond*/, bool& aIsCyclic, CNodeInstance* aBase)
{
	if (aIsCyclic)
	{
		return;
	}

	std::vector<SPin>& pins = aFirst->GetPins();
	for (auto& pin : pins)
	{
		if (pin.myPinType == SPin::PinTypeInOut::PinTypeInOut_OUT)
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

void CGraphManager::PreFrame(float aTimeDelta)
{
	static float timer = 0;
	timer += aTimeDelta;
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize({ io.DisplaySize.x / 2,  io.DisplaySize.y });
	ImGui::Begin("Content", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	if (ImGui::Button("Retrigger"))
	{
		ReTriggerTree();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		myLikeToSave = true;
		
	}
	static bool showFlow = false;
	if (ImGui::Checkbox("Show Flow", &showFlow))
	{
		myLikeToShowFlow = showFlow;
			
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		LoadTreeFromFile();
		ReTriggerTree();
	}

	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		nodeInstance->DebugUpdate();
		nodeInstance->VisualUpdate(aTimeDelta);
	}

	static int outRate = 30;
	ImGui::SliderInt("Max framerate", &outRate, 0, 100);
	if (timer > (1.0f / (float)outRate))
	{
		ReTriggerUpdateringTrees();
		timer = 0;
	}


	ed::SetCurrentEditor(g_Context);
	ed::Begin("My Editor", ImVec2(0.0, 0.0f));
}

bool ArePinTypesCompatible(SPin& aFirst, SPin& aSecond)
{
	if ((aFirst.myVariableType == SPin::PinType::Flow && aSecond.myVariableType != SPin::PinType::Flow)	)
	{
		return false;
	}
	if ((aSecond.myVariableType == SPin::PinType::Flow && aFirst.myVariableType != SPin::PinType::Flow))
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
	for (auto &nodeInstance : myNodeInstancesInGraph)
	{	
		if (!nodeInstance->myHasSetEditorPos)
		{
			ed::SetNodePosition(nodeInstance->myUID.AsInt(), ImVec2(nodeInstance->myEditorPos[0], nodeInstance->myEditorPos[1]));
			nodeInstance->myHasSetEditorPos = true;
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
				if (pin.myPinType == SPin::PinTypeInOut::PinTypeInOut_OUT)
				{
					isFirstInput = false;
				}
				isFirstIteration = false;
			}
			if (pin.myPinType == SPin::PinTypeInOut::PinTypeInOut_IN)
			{
				ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Input);
				
				ImGui::Text(pin.myText.c_str());
				ImGui::SameLine(0, 0);
				if (pin.myVariableType == SPin::PinType::Flow)
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
			
			const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth/* * 0.5f*/;
			auto headerColor = nodeInstance->GetColor(); 
			static ImTextureID HeaderTextureId = ImGui_LoadTexture("Sprites/BlueprintBackground.png");
			const auto uv = ImVec2(
				HeaderRect.w / (float)(4.0f * ImGui_GetTextureWidth(HeaderTextureId)),
				HeaderRect.h / (float)(4.0f * ImGui_GetTextureWidth(HeaderTextureId)));

				drawList->AddImageRounded(HeaderTextureId,
					to_imvec(HeaderRect.top_left()) - ImVec2(8 - halfBorderWidth,  4 - halfBorderWidth),
					to_imvec(HeaderRect.bottom_right()) + ImVec2( 8 - halfBorderWidth, 0),
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
		ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

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
								if (firstPin->myPinType == SPin::PinTypeInOut::PinTypeInOut_IN && secondPin->myPinType == SPin::PinTypeInOut::PinTypeInOut_IN)
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
								if (secondPin->myVariableType == SPin::PinType::Unknown)
								{
									secondNode->ChangePinTypes(firstPin->myVariableType);
								}
								int linkId = myNextLinkIdCounter++;
								firstNode->AddLinkToVia(secondNode, static_cast<unsigned int>(inputPinId.Get()), static_cast<unsigned int>(outputPinId.Get()), linkId);
								secondNode->AddLinkToVia(firstNode, static_cast<unsigned int>(outputPinId.Get()), static_cast<unsigned int>(inputPinId.Get()), linkId);

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
									if (firstPin->myPinType == SPin::PinTypeInOut::PinTypeInOut_IN)
									{
										myLinks.push_back({ ed::LinkId(linkId), outputPinId, inputPinId });
									}
									else
									{
										myLinks.push_back({ ed::LinkId(linkId), inputPinId, outputPinId });
									}		

									std::cout << "push add link command!" << std::endl;
									myUndoCommands.push({ CommandAction::AddLink, firstNode, secondNode, myLinks.back(), 0});
									
									ReTriggerTree();
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
					if (link.Id == deletedLinkId)
					{
						CNodeInstance* firstNode = GetNodeFromPinID(static_cast<unsigned int>(link.InputId.Get()));
						CNodeInstance* secondNode = GetNodeFromPinID(static_cast<unsigned int>(link.OutputId.Get()));
						assert(firstNode);
						assert(secondNode);

						firstNode->RemoveLinkToVia(secondNode, static_cast<unsigned int>(link.InputId.Get()));
						secondNode->RemoveLinkToVia(firstNode, static_cast<unsigned int>(link.OutputId.Get()));

						if (myShouldPushCommand)
						{
							std::cout << "push remove link action!" << std::endl;
							myUndoCommands.push({ CommandAction::RemoveLink, firstNode, secondNode, link, 0/*static_cast<unsigned int>(link.Id.Get())*//*, static_cast<unsigned int>(link.InputId.Get()), static_cast<unsigned int>(link.OutputId.Get())*/ });
						}

						myLinks.erase(&link);

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

				auto it = myNodeInstancesInGraph.begin();
				while (it != myNodeInstancesInGraph.end())
				{
					if ((*it)->myUID.AsInt() == nodeId.Get())
					{

						if (myShouldPushCommand) 
						{
							std::cout << "Push delete command!" << std::endl;
							myUndoCommands.push({ CommandAction::Delete, (*it), nullptr,  {0,0,0}, (*it)->myUID.AsInt() });
						}
						
						it = myNodeInstancesInGraph.erase(it);
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

		for (int i = 0; i < noOfTypes; i++)
		{
			cats[types[i]->GetNodeTypeCategory()].push_back(types[i]);
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
				dist.myScore = uiLevenshteinDistance<std::string>(types[i]->GetNodeName(), myMenuSeachField);
			}

			std::sort(distanceResults.begin(), distanceResults.end(), less_than_key());

			int firstCost = static_cast<int>(distanceResults[0].myScore);
			for (int i = 0; i < distanceResults.size(); i++)
			{
				CNodeInstance* node = nullptr;
				if (ImGui::MenuItem(distanceResults[i].ourInstance->GetNodeName().c_str()))
				{
					node = new CNodeInstance();

					//int nodeType = i;
					node->myNodeType = distanceResults[i].ourInstance;
					node->ConstructUniquePins();
					ed::SetNodePosition(node->myUID.AsInt(), newNodePostion);
					node->myHasSetEditorPos = true;

					myNodeInstancesInGraph.push_back(node);

					if (myShouldPushCommand)
					{
						std::cout << "Push create command!" << std::endl;
						myUndoCommands.push({ CommandAction::Create, node, nullptr, {0,0,0}, node->myUID.AsInt()});
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
					for (int i = 0; i < category.second.size(); i++)
					{
						CNodeType* type = category.second[i];
						if (ImGui::MenuItem(type->GetNodeName().c_str()))
						{
							node = new CNodeInstance();

							//int nodeType = i;
							node->myNodeType = type;
							node->ConstructUniquePins();
							ed::SetNodePosition(node->myUID.AsInt(), newNodePostion);
							node->myHasSetEditorPos = true;

							myNodeInstancesInGraph.push_back(node);
							
							if (myShouldPushCommand)
							{
								std::cout << "Push create command!" << std::endl;
								myUndoCommands.push({CommandAction::Create, node, nullptr, {0,0,0}, node->myUID.AsInt()});
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
				case CGraphManager::CommandAction::Create:
					inverseCommand.myAction = CommandAction::Delete;
					ed::DeleteNode(command.myResourceUID);
					break;
				case CGraphManager::CommandAction::Delete:
					inverseCommand.myAction = CommandAction::Create;
					myNodeInstancesInGraph.push_back(command.myNodeInstance);
					break;
				case CGraphManager::CommandAction::AddLink:
					inverseCommand.myAction = CommandAction::RemoveLink;
					ed::DeleteLink(command.myEditorLinkInfo.Id);
					break;
				case CGraphManager::CommandAction::RemoveLink:
					inverseCommand.myAction = CommandAction::AddLink;
					command.myNodeInstance->AddLinkToVia(command.mySecondNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.InputId.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.OutputId.Get()), command.myResourceUID);
					command.mySecondNodeInstance->AddLinkToVia(command.myNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.OutputId.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.InputId.Get()), command.myResourceUID);

					firstPin = command.myNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.InputId.Get()));
					secondPin = command.mySecondNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.OutputId.Get()));

					if (firstPin->myPinType == SPin::PinTypeInOut::PinTypeInOut_IN)
						myLinks.push_back({ command.myEditorLinkInfo.Id, command.myEditorLinkInfo.InputId, command.myEditorLinkInfo.OutputId });
					else
						myLinks.push_back({ command.myEditorLinkInfo.Id, command.myEditorLinkInfo.OutputId, command.myEditorLinkInfo.InputId });
					ReTriggerTree();
					break;
				default:
					break;
				}
				std::cout << "undo!" << std::endl;
				myUndoCommands.pop();
				std::cout << "Push redo command!" << std::endl;
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
				case CGraphManager::CommandAction::Create:
					inverseCommand.myAction = CommandAction::Delete;
					ed::DeleteNode(command.myResourceUID);
					break;
				case CGraphManager::CommandAction::Delete:
					inverseCommand.myAction = CommandAction::Create;
					myNodeInstancesInGraph.push_back(command.myNodeInstance);
					break;
				case CGraphManager::CommandAction::AddLink:
					inverseCommand.myAction = CommandAction::RemoveLink;
					ed::DeleteLink(command.myEditorLinkInfo.Id);
					break;
				case CGraphManager::CommandAction::RemoveLink:
					inverseCommand.myAction = CommandAction::AddLink;
					command.myNodeInstance->AddLinkToVia(command.mySecondNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.InputId.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.OutputId.Get()), command.myResourceUID);
					command.mySecondNodeInstance->AddLinkToVia(command.myNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.OutputId.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.InputId.Get()), command.myResourceUID);

					firstPin = command.myNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.InputId.Get()));
					secondPin = command.mySecondNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.OutputId.Get()));

					if (firstPin->myPinType == SPin::PinTypeInOut::PinTypeInOut_IN)
						myLinks.push_back({ command.myEditorLinkInfo.Id, command.myEditorLinkInfo.InputId, command.myEditorLinkInfo.OutputId });
					else
						myLinks.push_back({ command.myEditorLinkInfo.Id, command.myEditorLinkInfo.OutputId, command.myEditorLinkInfo.InputId });
					ReTriggerTree();
					break;
				default:
					break;
				}
				std::cout << "redo!" << std::endl;
				myRedoCommands.pop();
				std::cout << "Push undo command!" << std::endl;
				myUndoCommands.push(inverseCommand);
			}
		}
	}
}

void CGraphManager::Update()
{
	//float x = ImGui::GetMousePos().x;
	//float y = ImGui::GetMousePos().y;

	//std::cout << "x: " << x << " y: " << y << std::endl;

	//Input::Get()->IsHotKeyPressed(Input::EHotKeyButton::ECtrlZ);
	//Input::Get()->IsHotKeyPressed(Input::EHotKeyButton::ECtrlY);
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
			ed::Flow(myLinks[i].Id);
		}
	}
	for (auto i : myFlowsToBeShown)
	{
		ed::Flow(i);
	}
	myFlowsToBeShown.clear();
	ed::End();
	ed::SetCurrentEditor(nullptr);
	ImGui::End();
}
