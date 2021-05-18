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

class CGraphManager;
class CGameObject;
class CNodeType;

struct SNodeInstanceLink
{
	SNodeInstanceLink(class CNodeInstance* aLink, unsigned int aFromPinID, unsigned int aToPinID, unsigned int aLinkID)
		:myLink(aLink), myFromPinID(aFromPinID), myToPinID(aToPinID), myLinkID(aLinkID)
	{}
	
	class CNodeInstance* myLink = nullptr;
	unsigned int myFromPinID = UNDEFINED_PIN_ID;
	unsigned int myToPinID = UNDEFINED_PIN_ID;
	unsigned int myLinkID = 0;
};

class CNodeInstance
{
public:
	CNodeInstance(CGraphManager* aGraphManager, bool aCreateNewUID = true);

	CGameObject* GetCurrentGameObject();
	const CGraphManager* GraphManager() { return myGraphManager; }
	std::string GetNodeName() {	return myNodeType->NodeName(); }
	std::vector<SPin>& GetPins() { return myPins; }
	std::vector<SNodeInstanceLink>& GetLinks() { return myLinks; }
	CUID UID() { return myUID; }
	const float EditorPosition(const unsigned int& anIndex) const { return myEditorPosition[anIndex]; }
	void EditorPosition(const unsigned int& anIndex, const float& aValue) { myEditorPosition[anIndex] = aValue; }
	void HasSetEditorPosition(const bool& aHasSetEditorPosition) { myHasSetEditorPosition = aHasSetEditorPosition; }
	const bool HasSetEditorPosition() const { return myHasSetEditorPosition; }
	CNodeType* NodeType() const { return myNodeType; }
	void NodeType(CNodeType* aNodeType) { myNodeType = aNodeType; }

	bool IsPinConnected(SPin& aPin) { return GetLinkFromPin(aPin.myUID.AsInt()).size() > 0; }
	void CheckIfInputNode() { if (myNodeType->IsInputNode())myShouldTriggerAgain = false; }
	
	inline std::string WriteVariableType(const SPin& aPin) const
	{
		switch (aPin.myVariableType)
		{
		case SPin::EPinType::EBool:
		{
			return "BOOL";
		}
			break;
		case SPin::EPinType::EInt:
		{
			return "INT";
		}
			break;
		case SPin::EPinType::EFloat:
		{
			return "FLOAT";
		}
			break;
		case SPin::EPinType::EString:
		{
			return "STRING";
		}
			break;
		case SPin::EPinType::EVector3:
		{
			return "VECTOR3";
		}
			break;
		default:
			return "";
			break;
		}
	}
	
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
			else
			{
				switch (aPin.myVariableType)
				{
				case SPin::EPinType::EBool:
				{
					aWriter.Bool(NodeData::Get<bool>(aPin.myData));
				}
				break;
				case SPin::EPinType::EInt:
				{
					aWriter.Int(NodeData::Get<int>(aPin.myData));
				}
				break;
				case SPin::EPinType::EFloat:
				{
					aWriter.Double((double)NodeData::Get<float>(aPin.myData));
				}
				break;
				case SPin::EPinType::EString:
				{
					aWriter.String((char*)aPin.myData);
				}
				break;
				case SPin::EPinType::EVector3:
				{
					Vector3 data = NodeData::Get<Vector3>(aPin.myData);
					aWriter.StartArray();
					aWriter.Double(static_cast<double>(data.x));
					aWriter.Double(static_cast<double>(data.y));
					aWriter.Double(static_cast<double>(data.z));
					aWriter.EndArray();
				}
				break;
				default:
					aWriter.String("");
					break;
				}
			}
		}
	}
	template <typename Writer>
	void Serialize(Writer& aWriter) const
	{
		aWriter.StartObject();
		
		aWriter.Key("NodeType ID");
		aWriter.Int(myNodeType->myID);
		
		aWriter.Key("NodeType");
		aWriter.Int(static_cast<int>(myNodeType->myNodeType));

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
	
	void Enter();
	void ExitVia(unsigned int aPinIndex);

	void DebugUpdate();
	void VisualUpdate(float aDeltaTime);
	
	bool CanAddLink(unsigned int aPinIdFromMe);
	bool HasLinkBetween(unsigned int aFirstPin, unsigned int aSecondPin);
	bool AddLinkToVia(CNodeInstance* aLink, unsigned int aPinIdFromMe, unsigned int aPinIdToMe, unsigned int aLinkID);
	void RemoveLinkToVia(CNodeInstance* aLink, unsigned int aPinThatIOwn);

	void ConstructUniquePins();
	void ChangePinTypes(SPin::EPinType aType);
	void FetchData(SPin::EPinType& anOutType, NodeDataPtr& someData, size_t& anOutSize, unsigned int aPinToFetchFrom);
	std::vector< SNodeInstanceLink*> GetLinkFromPin(unsigned int aPinToFetchFrom);
	SPin* GetPinFromID(unsigned int aUID);
	int GetPinIndexFromPinUID(unsigned int aPinUID);

private:
	CGraphManager* myGraphManager;
	CNodeType* myNodeType;

	CUID myUID;
	
	float myEnteredTimer = 0.0f;
	float myEditorPosition[2];

	bool myHasSetEditorPosition = false;
	bool myShouldTriggerAgain = true;

	std::vector<SNodeInstanceLink> myLinks;
	std::vector<SPin> myPins;

	std::unordered_map<std::string, std::any> myMetaData;
};

