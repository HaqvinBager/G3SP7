#pragma once
#include "NodeTypes.h"
#include "NodeType.h"
#include <vector>
#include <any>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <sstream>
#include "imgui.h"
#include <imgui_node_editor.h>

#define UNDEFINED_PIN_ID INT_MAX

namespace ed = ax::NodeEditor;

struct SNodeInstanceLink
{
	SNodeInstanceLink(class CNodeInstance* aLink, unsigned int aFromPinID, unsigned int aToPinID, unsigned int aLinkID)
		:myLink(aLink), myFromPinID(aFromPinID), myToPinID(aToPinID), myLinkID(aLinkID)
	{

	}
	class CNodeInstance* myLink = nullptr;
	unsigned int myFromPinID = UNDEFINED_PIN_ID;
	unsigned int myToPinID = UNDEFINED_PIN_ID;
	unsigned int myLinkID = 0;
};

class CGraphManager;
class CGameObject;

class CNodeInstance
{
public:
	CNodeInstance(CGraphManager* aGraphManager, bool aCreateNewUID = true);
	CUID myUID;
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

	std::vector<SPin>& GetPins() { return myPins; }
	void ChangePinTypes(SPin::EPinType aType);

	void FetchData(SPin::EPinType& anOutType, NodeDataPtr& someData, size_t& anOutSize, unsigned int aPinToFetchFrom);
	std::vector<SNodeInstanceLink>& GetLinks() { return myLinks; }
	std::vector< SNodeInstanceLink*> GetLinkFromPin(unsigned int aPinToFetchFrom);
	SPin* GetPinFromID(unsigned int aUID);
	int GetPinIndexFromPinUID(unsigned int aPinUID);

	inline std::string WriteVariableType(const SPin& aPin) const
	{
		if (aPin.myVariableType == SPin::EPinType::EBool)
		{
			return "BOOL";
		}
		else if (aPin.myVariableType == SPin::EPinType::EInt)
		{
			return "INT";
		}
		else if (aPin.myVariableType == SPin::EPinType::EFloat)
		{
			return "FLOAT";
		}
		else if (aPin.myVariableType == SPin::EPinType::EString)
		{
			return "STRING";
		}
		return "";
	}


	template <typename Writer>
	inline void WritePinValue(Writer& aWriter, const SPin& aPin) const
	{

		if (aPin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_OUT)
		{
			aWriter.String("");
		}
		else
		{
			if (aPin.myData == NULL)
			{
				aWriter.String("");
			}
			else if (aPin.myVariableType == SPin::EPinType::EBool)
			{
				aWriter.Bool(NodeData::Get<bool>(aPin.myData));
			}
			else if (aPin.myVariableType == SPin::EPinType::EInt)
			{
				aWriter.Int(NodeData::Get<int>(aPin.myData));
			}
			else if (aPin.myVariableType == SPin::EPinType::EFloat)
			{
				aWriter.Double((double)NodeData::Get<float>(aPin.myData));

			}
			else if (aPin.myVariableType == SPin::EPinType::EString)
			{
				aWriter.String((char*)aPin.myData);
			}
			else
			{
				aWriter.String("");
			}
		}
	}

	template <typename Writer>
	void Serialize(Writer& aWriter) const
	{
		aWriter.StartObject();
		aWriter.Key("NodeType");
		aWriter.Int(myNodeType->myID);

		aWriter.Key("UID");
		aWriter.Int(myUID.AsInt());

		ImVec2 peditorPos = ed::GetNodePosition(myUID.AsInt());
		aWriter.Key("Position");
		aWriter.StartObject();
		{
			aWriter.Key("X");
			aWriter.Int(static_cast<int>(peditorPos.x));
			aWriter.Key("Y");
			aWriter.Int(static_cast<int>(peditorPos.y));
		}
		aWriter.EndObject();

		aWriter.Key("Pins");
		aWriter.StartArray();
		for (int i = 0; i < myPins.size(); i++)
		{
			//writer.Key("Pin");
			aWriter.StartObject();
			{
				aWriter.Key("Index");
				aWriter.Int(i);
				aWriter.Key("UID");
				aWriter.Int(static_cast<int>(myPins[i].myUID.AsInt()));
				aWriter.Key("DATA");
				WritePinValue(aWriter, myPins[i]);
				aWriter.Key("DATA_TYPE");
				aWriter.String(WriteVariableType(myPins[i]).c_str());
			}
			aWriter.EndObject();

		}
		aWriter.EndArray();


		aWriter.EndObject();
	}

	void DebugUpdate();
	void VisualUpdate(float aDeltaTime);
	float myEditorPosition[2];
	bool myHasSetEditorPosition = false;
	bool myShouldTriggerAgain = false;
	std::unordered_map<std::string, std::any> myMetaData;
	std::vector<SNodeInstanceLink> myLinks;
	std::vector<SPin> myPins;

	virtual int GetColor()
	{
		if (myEnteredTimer > 0.0f)
		{
			return COL32(0, min(myEnteredTimer * 200, 200), 0, 255);
		}
		if (myNodeType->IsStartNode())
		{
			return COL32(200, 75, 75, 255);
		}
		if (myNodeType->IsFlowNode())
		{
			return COL32(75, 125, 200, 255);
		}
		return COL32(75, 75, 75, 255);
	}
	float myEnteredTimer = 0.0f;

	std::vector<CGameObject*> GetCurrentGameObject();

private:
	CGraphManager* myGraphManager;
};

