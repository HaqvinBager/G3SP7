#pragma once
#include "NodeTypes.h"
#include "CNodeType.h"
#include <vector>
#include <any>
#include "..\rapidjson/writer.h"
#include "..\rapidjson/stringbuffer.h"
#include <sstream>
#include "imgui.h"
#include "Node Editor/imgui_node_editor.h"

#define UNDEFINED_PIN_ID INT_MAX 

namespace ed = ax::NodeEditor;

struct SNodeInstanceLink
{
	SNodeInstanceLink(class CNodeInstance* aLink, unsigned int aFrom, unsigned int aTo, unsigned int aLinkID)
		:myLink(aLink), myFromPinID(aFrom), myToPinID(aTo), myLinkID(aLinkID)
	{

	}
	class CNodeInstance* myLink = nullptr;
	unsigned int myFromPinID = UNDEFINED_PIN_ID;
	unsigned int myToPinID = UNDEFINED_PIN_ID;
	unsigned int myLinkID = 0;
};

class CNodeInstance
{
public:
	CNodeInstance(bool aCreateNewUID = true);
	UID myUID;
	class CNodeType* myNodeType;
	void Enter();
	void ExitVia(unsigned int aPinIndex);
	void ConstructUniquePins();

	bool CanAddLink(unsigned int aPinIdFromMe);
	bool HasLinkBetween(unsigned int aFirstPin, unsigned int aSecondPin);
	bool AddLinkToVia(CNodeInstance* aLink, unsigned int aPinIdFromMe, unsigned int aPinIdToMe, unsigned int aLinkID);
	void RemoveLinkToVia(CNodeInstance* aLink, unsigned int aPinThatIOwn);

	bool IsPinConnected(SPin& aPin);
	std::string GetNodeName();

	std::vector<SPin>& GetPins() {return myPins;}
	void ChangePinTypes(SPin::PinType aType);

	void FetchData(SPin::PinType& outType, NodeDataPtr& someData, size_t& outSize, unsigned int aPinToFetchFrom);
	std::vector<SNodeInstanceLink>& GetLinks() { return myLinks; }
	std::vector< SNodeInstanceLink*> GetLinkFromPin(unsigned int aPinToFetchFrom);
	SPin* GetPinFromID(unsigned int aUID);
	int GetPinIndexFromPinUID(unsigned int aPinUID);

	inline std::string WriteVariableType(const SPin& aPin) const
	{
		if (aPin.myVariableType == SPin::PinType::Bool)
		{
			return "BOOL";
		}
		else if (aPin.myVariableType == SPin::PinType::Int)
		{
			return "INT";
		}
		else if (aPin.myVariableType == SPin::PinType::Float)
		{
			return "FLOAT";
		}
		else if (aPin.myVariableType == SPin::PinType::String)
		{
			return "STRING";
		}
		return "";
	}


	template <typename Writer>
	inline void WritePinValue(Writer& writer, const SPin& aPin) const
	{
		if (aPin.myPinType == SPin::PinTypeInOut::PinTypeInOut_OUT)
		{
			writer.String("");
		}
		else
		{
			if (aPin.myVariableType == SPin::PinType::Bool)
			{
				writer.Bool(NodeData::Get<bool>(aPin.myData));
			}
			else if (aPin.myVariableType == SPin::PinType::Int)
			{
				writer.Int(NodeData::Get<int>(aPin.myData));
			}
			else if (aPin.myVariableType == SPin::PinType::Float)
			{
				writer.Double((double)NodeData::Get<float>(aPin.myData));

			}
			else if (aPin.myVariableType == SPin::PinType::String)
			{
				writer.String((char*)aPin.myData);
			}
			else
			{
				writer.String("");
			}
		}

	}

	template <typename Writer>
	void Serialize(Writer& writer) const 
	{
		writer.StartObject();
		writer.Key("NodeType");
		writer.Int(myNodeType->myID);

		writer.Key("UID");
		writer.Int(myUID.AsInt());

		ImVec2 peditorPos = ed::GetNodePosition(myUID.AsInt());
		writer.Key("Position");
		writer.StartObject();
		{
			writer.Key("X");
			writer.Int(static_cast<int>(peditorPos.x));
			writer.Key("Y");
			writer.Int(static_cast<int>(peditorPos.y));
		}
		writer.EndObject();

		writer.Key("Pins");
		writer.StartArray();
		for (int i=0; i< myPins.size(); i++)
		{
			//writer.Key("Pin");
			writer.StartObject();
			{
				writer.Key("Index");
				writer.Int(i);
				writer.Key("UID");
				writer.Int(static_cast<int>(myPins[i].myUID.AsInt()));
				writer.Key("DATA");
				WritePinValue(writer, myPins[i]);
				writer.Key("DATA_TYPE");
				writer.String(WriteVariableType(myPins[i]).c_str());
			}
			writer.EndObject();

		}
		writer.EndArray();


		writer.EndObject();
	}

	void DebugUpdate();
	void VisualUpdate(float aTimeDelta);
	float myEditorPos[2];
	bool myHasSetEditorPos = false;
	bool myShouldTriggerAgain = false;
	std::unordered_map<std::string, std::any> myMetaData;
	std::vector<SNodeInstanceLink> myLinks;
	std::vector<SPin> myPins;

	virtual int GetColor()
	{
		if (myEnteredTimer > 0.0f)
		{
			return COL32(0, min(myEnteredTimer * 255, 255), 0, 255);
		}
		if (myNodeType->IsStartNode())
		{
			return COL32(255, 128, 128, 255);
		}
		if (myNodeType->IsFlowNode())
		{
			return COL32(128, 195, 248, 255);
		}
		return COL32(255, 255, 255, 255);
	}
	float myEnteredTimer = 0.0f;
private:
	
};

